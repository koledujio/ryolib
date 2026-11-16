cbuffer ColorBuffer : register(b0) {
    float4 u_color;
};

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

struct PSInput {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET {
    float4 texColor = g_texture.Sample(g_sampler, input.uv);
    return texColor * u_color;
}