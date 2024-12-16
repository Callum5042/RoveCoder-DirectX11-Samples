// Vertex input
struct VertexInput
{
    float3 position : POSITION;
    float2 tex : TEXTURE;
};

// Pixel input structure
struct PixelInput
{
    float4 position : SV_POSITION;
    float3 positionWorld : POSITION;
    float2 tex : TEXTURE;
};

// World constant buffer
cbuffer WorldBuffer : register(b0)
{
    matrix cModelViewProjection;
    matrix cModelWorld;
}

cbuffer FogBuffer : register(b1)
{
    float3 cCameraEyePosition;
    float cFogStart;
    float cFogRange;
}

// Texture sampler
SamplerState gTextureSampler : register(s0);

// Textures
Texture2D gTextureDiffuse : register(t0);