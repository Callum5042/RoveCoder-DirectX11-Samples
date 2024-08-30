// Vertex input
struct VertexInput
{
    float3 position : POSITION;
    float2 tex : TEXTURE;
    float3 normal : NORMAL;
};

// Pixel input structure
struct PixelInput
{
    float4 positionClipSpace : SV_POSITION;
    float3 position : POSITION;
    float2 tex : TEXTURE;
    float3 normal : NORMAL;
};

// World constant buffer
cbuffer WorldBuffer : register(b0)
{
    matrix cModelViewProjection;
    matrix cModel;
    matrix cModelInverse;
    float4 cCameraPosition;
}

struct Attenuation
{
    float constant;
    float linear_;
    float quadratic;
};

// Directional light constant buffer
cbuffer DirectionalLightBuffer : register(b1)
{
    float3 cLightPosition;
    float _padding1;
    Attenuation CLightAttenuate;
    float _padding2;
}