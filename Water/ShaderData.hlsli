// Vertex input
struct VertexInput
{
    float3 position : POSITION;
    float2 tex : TEXTURE;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

// Pixel input structure
struct PixelInput
{
    float4 position : SV_POSITION;
    float2 tex1 : TEXTURE1;
    float2 tex2 : TEXTURE2;
    
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

// World constant buffer
cbuffer WorldBuffer : register(b0)
{
    matrix cModelViewProjection;
}

// Texture
cbuffer TextureBuffer : register(b1)
{
    matrix cTextureMatrix1;
    matrix cTextureMatrix2;
}

// Texture sampler
SamplerState gTextureSampler : register(s0);

// Textures
Texture2D gTextureDiffuse1 : register(t0);
Texture2D gTextureDiffuse2 : register(t1);