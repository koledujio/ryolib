#include <metal_stdlib>
using namespace metal;

struct VertexInput {
    float3 position [[attribute(0)]];
    float2 uv       [[attribute(1)]];
};

struct VertexOut {
    float4 position [[position]];
    float2 uv;
};

vertex VertexOut texture_vertex_main(VertexInput in [[stage_in]], constant float4x4& mvp [[buffer(1)]]) {
    VertexOut out;
    out.position = mvp * float4(in.position, 1.0);
    out.uv = in.uv;
    return out;
}

fragment float4 texture_fragment_main(VertexOut in [[stage_in]], texture2d<float> tex [[texture(0)]], sampler texSampler [[sampler(0)]], constant float4& color [[buffer(0)]]) {
    float4 texColor = tex.sample(texSampler, in.uv);
    return texColor * color;
}