import Metal
import MetalKit
import SwiftUI

/// Renders a single **synthetic** precompiled-shader triangle using a small
/// Direct3D 9 state bridge. This does not load IW5 commands or game shaders.
final class MetalSmokeRenderer: NSObject, MTKViewDelegate {
    private let queue: MTLCommandQueue
    private let pipeline: MTLRenderPipelineState
    private var d3d9 = MW3D3D9State()

    init?(device: MTLDevice) {
        guard let queue = device.makeCommandQueue(),
              let library = device.makeDefaultLibrary(),
              let vs = library.makeFunction(name: "mw3ProbeVertex"),
              let fs = library.makeFunction(name: "mw3ProbeFragment")
        else { return nil }
        self.queue = queue
        mw3_d3d9_reset(&d3d9)

        let desc = MTLRenderPipelineDescriptor()
        desc.vertexFunction = vs
        desc.fragmentFunction = fs
        guard let attachment = desc.colorAttachments[0] else { return nil }
        attachment.pixelFormat = .bgra8Unorm

        attachment.writeMask = MetalSmokeRenderer.writeMask(d3d9.color_write_mask)
        attachment.isBlendingEnabled = d3d9.alpha_blend_enabled != 0
        attachment.sourceRGBBlendFactor = MetalSmokeRenderer.blendFactor(d3d9.src_blend)
        attachment.destinationRGBBlendFactor = MetalSmokeRenderer.blendFactor(d3d9.dst_blend)
        attachment.rgbBlendOperation = MetalSmokeRenderer.blendOp(d3d9.blend_op)
        attachment.sourceAlphaBlendFactor = MetalSmokeRenderer.blendFactor(
            d3d9.separate_alpha_enabled != 0 ? d3d9.src_alpha_blend : d3d9.src_blend
        )
        attachment.destinationAlphaBlendFactor = MetalSmokeRenderer.blendFactor(
            d3d9.separate_alpha_enabled != 0 ? d3d9.dst_alpha_blend : d3d9.dst_blend
        )
        attachment.alphaBlendOperation = MetalSmokeRenderer.blendOp(
            d3d9.separate_alpha_enabled != 0 ? d3d9.alpha_blend_op : d3d9.blend_op
        )

        guard let pipeline = try? device.makeRenderPipelineState(descriptor: desc)
        else { return nil }
        self.pipeline = pipeline
        super.init()
    }

    private static func writeMask(_ bits: UInt32) -> MTLColorWriteMask {
        var mask: MTLColorWriteMask = []
        if bits & 1 != 0 { mask.insert(.red) }
        if bits & 2 != 0 { mask.insert(.green) }
        if bits & 4 != 0 { mask.insert(.blue) }
        if bits & 8 != 0 { mask.insert(.alpha) }
        return mask
    }

    private static func blendFactor(_ d3d: UInt32) -> MTLBlendFactor {
        switch d3d {
        case 1: return .zero
        case 2: return .one
        case 3: return .sourceColor
        case 4: return .oneMinusSourceColor
        case 5: return .sourceAlpha
        case 6: return .oneMinusSourceAlpha
        case 7: return .destinationAlpha
        case 8: return .oneMinusDestinationAlpha
        case 9: return .destinationColor
        case 10: return .oneMinusDestinationColor
        case 11: return .sourceAlphaSaturated
        default: return .one // The C bridge rejects unrecognized blend factors.
        }
    }

    private static func blendOp(_ d3d: UInt32) -> MTLBlendOperation {
        switch d3d {
        case 2: return .subtract
        case 3: return .reverseSubtract
        case 4: return .min
        case 5: return .max
        default: return .add
        }
    }

    func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) {}

    func draw(in view: MTKView) {
        guard let drawable = view.currentDrawable,
              let pass = view.currentRenderPassDescriptor,
              let buffer = queue.makeCommandBuffer(),
              let encoder = buffer.makeRenderCommandEncoder(descriptor: pass)
        else { return }

        // D3DCULL_CCW culls CCW triangles; front-facing triangles are CW.
        // Metal's front-face setting is explicit and geometry-dependent.
        if d3d9.cull_mode == 1 {
            encoder.setCullMode(.none)
        } else {
            encoder.setFrontFacing(d3d9.cull_mode == 3 ? .clockwise : .counterClockwise)
            encoder.setCullMode(.back)
        }

        encoder.setRenderPipelineState(pipeline)
        encoder.drawPrimitives(type: .triangle, vertexStart: 0, vertexCount: 3)
        encoder.endEncoding()
        buffer.present(drawable)
        buffer.commit()
    }
}

/// Keeps a selectable internal drawable resolution independent of HUD layout.
final class ScaledMetalProbeView: MTKView {
    var renderScale: CGFloat = 1.0 {
        didSet { if renderScale != oldValue { setNeedsLayout() } }
    }

    override func didMoveToWindow() {
        super.didMoveToWindow()
        setNeedsLayout()
    }

    override func layoutSubviews() {
        super.layoutSubviews()
        let nativeScale = window?.screen.scale ?? UIScreen.main.scale
        let pixels = CGSize(
            width: max(1, (bounds.width * nativeScale * renderScale).rounded()),
            height: max(1, (bounds.height * nativeScale * renderScale).rounded())
        )
        if drawableSize != pixels { drawableSize = pixels }
    }
}

struct MetalProbeView: UIViewRepresentable {
    let quality: MW3QualityChoice

    final class Coordinator {
        var renderer: MetalSmokeRenderer?
    }

    private var internalScale: CGFloat {
        switch quality {
        case .performance: return 0.60
        case .balanced: return 0.75
        case .quality: return 0.90
        case .nearNative: return 1.00
        }
    }

    func makeCoordinator() -> Coordinator { Coordinator() }

    func makeUIView(context: Context) -> ScaledMetalProbeView {
        let view = ScaledMetalProbeView(frame: .zero, device: MTLCreateSystemDefaultDevice())
        view.colorPixelFormat = .bgra8Unorm
        view.clearColor = MTLClearColor(red: 0.025, green: 0.045, blue: 0.08, alpha: 1)
        view.framebufferOnly = true
        view.autoResizeDrawable = false
        view.isPaused = false
        view.renderScale = internalScale
        if let device = view.device, let renderer = MetalSmokeRenderer(device: device) {
            context.coordinator.renderer = renderer
            view.delegate = renderer
        }
        return view
    }

    func updateUIView(_ view: ScaledMetalProbeView, context: Context) {
        view.renderScale = internalScale
    }
}
