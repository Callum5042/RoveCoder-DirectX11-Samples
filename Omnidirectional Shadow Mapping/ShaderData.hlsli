// Vertex input
struct VertexInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

// Pixel input structure
struct PixelInput
{
    float4 positionClipSpace : SV_POSITION;
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float4 lightViewProjection : TEXCOORD1;
};

// World constant buffer
cbuffer ModelBuffer : register(b0)
{
    matrix cModelTransform;
    matrix cModelTransformInverse;
}

cbuffer CameraBuffer : register(b1)
{
    matrix cCameraView;
    matrix cCameraProjection;
    float4 cCameraPosition;
}

// Directional light constant buffer
cbuffer DirectionalLightBuffer : register(b2)
{
    float4 cLightDirection;
    matrix cLightView;
    matrix cLightProjection;
}

// Shadow map
Texture2D gShadowMap : register(t0);
SamplerComparisonState gShadowSampler : register(s0);