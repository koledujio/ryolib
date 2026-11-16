cbuffer ColorBuffer : register(b0) {
    float4 u_color;
};

struct PSInput {
    float4 position : SV_POSITION;
};

float4 main(PSInput input) : SV_TARGET {
    return u_color;
}