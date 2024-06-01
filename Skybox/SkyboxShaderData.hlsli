// Vertex input
struct VertexInput
{
    float3 position : POSITION;
    float2 tex : TEXTURE;
};

// Pixel input structure
struct PixelInput
{
    float4 positionClipSpace : SV_POSITION;
    float3 position : POSITION;
    float2 tex : TEXTURE;
};

// World constant buffer
cbuffer WorldBuffer : register(b0)
{
    matrix cModelViewProjection;
}

// Texture sampler
SamplerState gTextureSampler : register(s0);

// Textures
TextureCube gTextureDiffuse : register(t0);