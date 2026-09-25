cbuffer StencilVertexUniforms : register(b0) {
    matrix u_matrix;
};

struct VSInput {
    float3 position : POSITION;
};

struct PSInput {
    float4 position : SV_POSITION;
    float2 localPosition : TEXCOORD0;
};

PSInput main(VSInput input) {
    PSInput output;
    output.position = mul(u_matrix, float4(input.position, 1.0f));
    output.localPosition = input.position.xy;
    return output;
}
