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
    float2 tex : TEXTURE;
};

// World constant buffer
cbuffer WorldBuffer : register(b0)
{
    matrix cModelViewProjection;
    int4 padding;
}

// Texture sampler
SamplerState gTextureSampler : register(s0);

// Textures
Texture2D gTextureDiffuse1 : register(t0);
Texture2D gTextureDiffuse2 : register(t1);
Texture2D gTextureDiffuse3 : register(t2);