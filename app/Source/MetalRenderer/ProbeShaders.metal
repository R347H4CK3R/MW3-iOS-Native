#include <metal_stdlib>
using namespace metal;

// This standalone synthetic shader is compiled into the iOS app's metallib.
// It does NOT originate from MW3 and does NOT implement IW5 vertex/pixel shaders.
struct MW3ProbeVertex {
    float4 position [[position]];
    float4 color;
};

vertex MW3ProbeVertex mw3ProbeVertex(uint vertexIndex [[vertex_id]]) {
    constexpr float2 vertices[3] = {
        float2(-0.7f, -0.6f),
        float2( 0.0f,  0.6f),
        float2( 0.7f, -0.6f)
    };
    constexpr float4 colors[3] = {
        float4(0.18f, 0.7f, 0.95f, 1.0f),
        float4(0.95f, 0.4f, 0.28f, 1.0f),
        float4(0.3f, 0.95f, 0.55f, 1.0f)
    };
    MW3ProbeVertex out;
    out.position = float4(vertices[vertexIndex], 0.5f, 1.0f);
    out.color = colors[vertexIndex];
    return out;
}

fragment float4 mw3ProbeFragment(MW3ProbeVertex in [[stage_in]]) {
    return in.color;
}
