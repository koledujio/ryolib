#include <metal_stdlib>
using namespace metal;

struct VertexInput {
    float3 position [[attribute(0)]];
};

struct VertexOut {
    float4 position [[position]];
};

vertex VertexOut color_vertex_main(VertexInput in [[stage_in]], constant float4x4& mvp [[buffer(1)]]) {
    VertexOut out;
    out.position = mvp * float4(in.position, 1.0);
    return out;
}

fragment float4 color_fragment_main(VertexOut in [[stage_in]], constant float4& color [[buffer(0)]]) {
    return color;
}