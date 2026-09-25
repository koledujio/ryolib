cbuffer StencilPixelUniforms : register(b0) {
    float2 u_scale;
    float u_radius;
    uint u_shape;
};

struct PSInput {
    float4 position : SV_POSITION;
    float2 localPosition : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET {
    float2 position = input.localPosition;

    switch (u_shape) {
        case 0: // RECTANGLE
            break;

        case 1: { // ROUNDED RECTANGLE
            float r = u_radius * min(u_scale.x, u_scale.y);
            float2 halfSize = 0.5f * u_scale;
            float2 q = abs(position * u_scale) - (halfSize - float2(r, r));
            float dist = length(max(q, float2(0.0f, 0.0f))) + min(max(q.x, q.y), 0.0f) - r;

            if (dist > 0.0f)
                discard;

            break;
        }

        case 2: { // OVAL
            float2 uv = position * 2.0f;

            if (dot(uv, uv) > 1.0f)
                discard;

            break;
        }
    }

    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
