import Metal
import MetalKit
import SwiftUI

/// Metal API smoke test: clears drawable only. This is NOT an IW5 renderer.
final class MetalSmokeRenderer: NSObject, MTKViewDelegate {
    private let queue: MTLCommandQueue

    init?(device: MTLDevice) {
        guard let queue = device.makeCommandQueue() else { return nil }
        self.queue = queue
        super.init()
    }

    func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) {}

    func draw(in view: MTKView) {
        guard let drawable = view.currentDrawable,
              let pass = view.currentRenderPassDescriptor,
              let buffer = queue.makeCommandBuffer(),
              let encoder = buffer.makeRenderCommandEncoder(descriptor: pass)
        else { return }
        encoder.endEncoding()
        buffer.present(drawable)
        buffer.commit()
    }
}

struct MetalProbeView: UIViewRepresentable {
    final class Coordinator {
        var renderer: MetalSmokeRenderer?
    }

    func makeCoordinator() -> Coordinator { Coordinator() }

    func makeUIView(context: Context) -> MTKView {
        let view = MTKView(frame: .zero, device: MTLCreateSystemDefaultDevice())
        view.colorPixelFormat = .bgra8Unorm
        view.clearColor = MTLClearColor(red: 0.025, green: 0.045, blue: 0.08, alpha: 1)
        view.framebufferOnly = true
        view.isPaused = false
        if let device = view.device, let renderer = MetalSmokeRenderer(device: device) {
            context.coordinator.renderer = renderer
            view.delegate = renderer
        }
        return view
    }

    func updateUIView(_ view: MTKView, context: Context) {}
}
