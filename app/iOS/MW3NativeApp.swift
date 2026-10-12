import SwiftUI
import UniformTypeIdentifiers

enum MW3ModeChoice: String, CaseIterable, Identifiable {
    case story = "Story"
    case multiplayer = "Multiplayer"
    case specOps = "Spec Ops"
    var id: String { rawValue }
}

enum MW3QualityChoice: String, CaseIterable, Identifiable {
    case performance = "Performance"
    case balanced = "Balanced"
    case quality = "Quality"
    case nearNative = "Near Native"
    var id: String { rawValue }
}

@main
struct MW3NativeApp: App {
    var body: some Scene {
        WindowGroup { MW3NativeShellView() }
    }
}

/// Native controls and file permissions smoke-test app. No game is integrated.
struct MW3NativeShellView: View {
    @StateObject private var files = GameFolderAccess()
    @StateObject private var inputs = ControllerInputs()
    @State private var choosingFolder = false
    @State private var selectedMode: MW3ModeChoice = .story
    @State private var selectedQuality: MW3QualityChoice = .balanced

    var body: some View {
        ZStack {
            MetalProbeView(quality: selectedQuality).ignoresSafeArea()
            Color.black.opacity(0.30).ignoresSafeArea()
            VStack(spacing: 10) {
                HStack(spacing: 20) {
                    Text("MW3 iOS — Input & Metal Test Shell")
                        .font(.headline).foregroundStyle(.white)
                    Spacer()
                    Picker("Mode", selection: $selectedMode) {
                        ForEach(MW3ModeChoice.allCases) { mode in
                            Text(mode.rawValue).tag(mode)
                        }
                    }.pickerStyle(.menu)
                    Picker("Graphics", selection: $selectedQuality) {
                        ForEach(MW3QualityChoice.allCases) { quality in
                            Text(quality.rawValue).tag(quality)
                        }
                    }.pickerStyle(.menu)
                }
                HStack(spacing: 12) {
                    Button("Select game folder") { choosingFolder = true }
                    Text(files.folderName).lineLimit(1)
                    Spacer()
                    Toggle("Gyro aim", isOn: Binding(
                        get: { inputs.gyroEnabled },
                        set: { inputs.setGyroEnabled($0) }
                    ))
                    .disabled(!inputs.gyroSupported)
                    .frame(width: 150)
                }
                .font(.caption)
                .foregroundStyle(.white)

                if let error = files.lastError {
                    Text(error).font(.caption2).foregroundStyle(.yellow)
                }

                Text("ENGINE NOT LINKED — No MW3 gameplay, shaders, or original aim assist in this test shell.")
                    .font(.caption).foregroundStyle(.yellow)

                Spacer()

                HStack(alignment: .bottom, spacing: 22) {
                    TouchPad(title: "MOVE", onInput: inputs.setTouchMove)
                    Spacer()
                    VStack(alignment: .leading, spacing: 5) {
                        Text(inputs.controllerConnected ? "Physical controller connected" : "Touch controls active")
                        Text(String(format: "Move %.2f / %.2f",
                                    inputs.state.move_x, inputs.state.move_y))
                        Text(String(format: "Look %.2f / %.2f",
                                    inputs.state.look_x, inputs.state.look_y))
                        Text("Current mode: " + selectedMode.rawValue)
                    }
                    .font(.caption2)
                    .monospacedDigit()
                    .foregroundStyle(.white)
                    Spacer()
                    TouchPad(title: "LOOK", onInput: inputs.setTouchLook)
                    VStack(spacing: 8) {
                        HoldButton(title: "FIRE", pressed: inputs.setTouchFire)
                        HoldButton(title: "AIM", pressed: inputs.setTouchAim)
                    }
                }
            }
            .padding(20)
        }
        .fileImporter(isPresented: $choosingFolder, allowedContentTypes: [.folder]) { result in
            if case let .success(url) = result {
                files.select(url)
            }
        }
        .onAppear { inputs.start() }
        .onDisappear { inputs.stop() }
    }
}

struct TouchPad: View {
    let title: String
    let onInput: (Float, Float) -> Void
    @State private var axisX: CGFloat = 0
    @State private var axisY: CGFloat = 0

    var body: some View {
        GeometryReader { geometry in
            ZStack {
                Circle().fill(Color.white.opacity(0.16))
                Circle().stroke(Color.white.opacity(0.60), lineWidth: 2)
                Circle()
                    .fill(Color.white.opacity(0.7))
                    .frame(width: 45, height: 45)
                    .offset(x: axisX * 35, y: axisY * 35)
                Text(title).font(.caption2).foregroundStyle(.white)
                    .offset(y: -55)
            }
            .contentShape(Circle())
            .gesture(DragGesture(minimumDistance: 0)
                .onChanged { value in
                    let cx = geometry.size.width / 2
                    let cy = geometry.size.height / 2
                    axisX = min(max((value.location.x - cx) / 55, -1), 1)
                    axisY = min(max((value.location.y - cy) / 55, -1), 1)
                    onInput(Float(axisX), Float(-axisY))
                }
                .onEnded { _ in
                    axisX = 0
                    axisY = 0
                    onInput(0, 0)
                })
        }
        .frame(width: 135, height: 135)
    }
}

struct HoldButton: View {
    let title: String
    let pressed: (Bool) -> Void
    var body: some View {
        Text(title)
            .font(.caption).bold()
            .foregroundStyle(.white)
            .frame(width: 65, height: 48)
            .background(Color.blue.opacity(0.7), in: RoundedRectangle(cornerRadius: 12))
            .contentShape(RoundedRectangle(cornerRadius: 12))
            .gesture(DragGesture(minimumDistance: 0)
                .onChanged { _ in pressed(true) }
                .onEnded { _ in pressed(false) })
    }
}
