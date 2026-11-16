#include <metal_stdlib>
using namespace metal;

struct VertexInput {
    float3 position [[attribute(0)]];
};

struct VertexOut {
    float4 position [[position]];
    float2 localPosition;
};

struct StencilVertexUniforms {
    float4x4 matrix;
};

struct StencilFragmentUniforms {
    float2 scale;
    float radius;
    uint shape;
};

vertex VertexOut stencil_vertex_main(VertexInput in [[stage_in]], constant StencilVertexUniforms& uniforms [[buffer(1)]]) {
    VertexOut out;

    out.position = uniforms.matrix * float4(in.position, 1.0);
    out.localPosition = in.position.xy;

    return out;
}

fragment float4 stencil_fragment_main(VertexOut in [[stage_in]], constant StencilFragmentUniforms& uniforms [[buffer(0)]]) {
    float2 position = in.localPosition;

    switch (uniforms.shape) {
        case 0: // RECTANGLE
            break;
        
        case 1: { // ROUNDED RECTANGLE
            float r = uniforms.radius * min(uniforms.scale.x, uniforms.scale.y);
            float2 halfSize = 0.5f * uniforms.scale;
            float2 q = abs(position * uniforms.scale) - (halfSize - float2(r, r));
            float dist = length(max(q, float2(0.0f, 0.0f))) + min(max(q.x, q.y), 0.0f) - r;

            if (dist > 0.0f)
                discard_fragment();
            
            break;
        }

        case 2: { // OVAL
            float2 uv = position * 2.0;

            if (dot(uv, uv) > 1.0)
                discard_fragment();
            
            break;
        }
    }

    return float4(1.0);
}