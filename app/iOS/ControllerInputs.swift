import Foundation
import Combine
import CoreMotion
import GameController

/// Experimental controller adapter. Real IW5 bindings and button meanings
/// must be verified before these values can drive original game code.
@MainActor
final class ControllerInputs: ObservableObject {
    @Published private(set) var state = MW3ControllerState()
    @Published private(set) var controllerConnected = false
    @Published private(set) var gyroEnabled = false

    private let motion = CMMotionManager()
    private var activeController: GCController?
    private var observers: [NSObjectProtocol] = []
    private var touchMove = SIMD2<Float>(repeating: 0)
    private var touchLook = SIMD2<Float>(repeating: 0)
    private var gyroLook = SIMD2<Float>(repeating: 0)
    private var touchFire = false
    private var touchAim = false

    var gyroSupported: Bool { motion.isGyroAvailable }

    func start() {
        if observers.isEmpty {
            observers.append(
                NotificationCenter.default.addObserver(
                    forName: .GCControllerDidConnect, object: nil, queue: .main
                ) { [weak self] note in
                    guard let controller = note.object as? GCController else { return }
                    Task { @MainActor in self?.attach(controller) }
                }
            )
            observers.append(
                NotificationCenter.default.addObserver(
                    forName: .GCControllerDidDisconnect, object: nil, queue: .main
                ) { [weak self] note in
                    guard let controller = note.object as? GCController else { return }
                    Task { @MainActor in
                        if self?.activeController === controller {
                            self?.activeController = nil
                            self?.controllerConnected = false
                            self?.refresh()
                        }
                    }
                }
            )
        }
        if let controller = GCController.controllers().first(where: { $0.extendedGamepad != nil }) {
            attach(controller)
        }
        refresh()
    }

    func stop() {
        setGyroEnabled(false)
        activeController?.extendedGamepad?.valueChangedHandler = nil
        activeController = nil
        for observer in observers { NotificationCenter.default.removeObserver(observer) }
        observers.removeAll()
        controllerConnected = false
        refresh()
    }

    private func attach(_ controller: GCController) {
        guard let gamepad = controller.extendedGamepad else { return }
        activeController?.extendedGamepad?.valueChangedHandler = nil
        activeController = controller
        controllerConnected = true
        gamepad.valueChangedHandler = { [weak self] _, _ in
            Task { @MainActor in self?.refresh() }
        }
        refresh()
    }

    func setTouchMove(x: Float, y: Float) {
        touchMove = SIMD2<Float>(x, y)
        refresh()
    }

    func setTouchLook(x: Float, y: Float) {
        touchLook = SIMD2<Float>(x, y)
        refresh()
    }

    func setTouchFire(_ pressed: Bool) {
        touchFire = pressed
        refresh()
    }

    func setTouchAim(_ pressed: Bool) {
        touchAim = pressed
        refresh()
    }

    func setGyroEnabled(_ enabled: Bool) {
        motion.stopGyroUpdates()
        gyroEnabled = enabled && motion.isGyroAvailable
        gyroLook = .zero
        if gyroEnabled {
            motion.gyroUpdateInterval = 1.0 / 60.0
            motion.startGyroUpdates(to: .main) { [weak self] sample, _ in
                guard let sample else { return }
                Task { @MainActor in
                    // Rotation rate affects look input ONLY, never movement or steering.
                    self?.gyroLook = SIMD2<Float>(
                        Float(sample.rotationRate.y * (1.0 / 60.0)),
                        Float(-sample.rotationRate.x * (1.0 / 60.0))
                    )
                    self?.refresh()
                }
            }
        }
        refresh()
    }

    private func choose(_ physical: Float, _ touch: Float) -> Float {
        return abs(physical) > 0.05 ? physical : touch
    }

    private func refresh() {
        let pad = activeController?.extendedGamepad
        var current = MW3ControllerState()
        current.move_x = choose(pad?.leftThumbstick.xAxis.value ?? 0, touchMove.x)
        current.move_y = choose(pad?.leftThumbstick.yAxis.value ?? 0, touchMove.y)
        current.look_x = choose(pad?.rightThumbstick.xAxis.value ?? 0, touchLook.x)
        current.look_y = choose(pad?.rightThumbstick.yAxis.value ?? 0, touchLook.y)
        current.left_trigger = max(pad?.leftTrigger.value ?? 0, touchAim ? 1 : 0)
        current.right_trigger = max(pad?.rightTrigger.value ?? 0, touchFire ? 1 : 0)
        // Development-only test flags: NOT validated IW5 controller bit values.
        current.buttons = (pad?.buttonA.isPressed == true ? 1 : 0) |
                          (pad?.buttonB.isPressed == true ? 2 : 0)
        var gyro = MW3GyroAimInput()
        gyro.delta_x = gyroLook.x
        gyro.delta_y = gyroLook.y
        gyro.sensitivity = 1
        gyro.enabled = gyroEnabled ? 1 : 0
        state = mw3_apply_gyro_aim(current, gyro)
    }
}
