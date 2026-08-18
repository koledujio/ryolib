cbuffer UniformBuffer : register(b1) {
    matrix u_mvp;
};

struct VSInput {
    float3 position : POSITION;
};

struct PSInput {
    float4 position : SV_POSITION;
};

PSInput main(VSInput input) {
    PSInput output;
    output.position = mul(u_mvp, float4(input.position, 1.0f));
    return output;
}