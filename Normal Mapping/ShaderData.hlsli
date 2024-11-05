// Vertex input
struct VertexInput
{
    float3 position : POSITION;
    float2 tex_coord : TEXTURE;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

// Pixel input structure
struct PixelInput
{
    float4 positionClipSpace : SV_POSITION;
    float3 position : POSITION;
    float2 tex_coord : TEXTURE;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

// World constant buffer
cbuffer WorldBuffer : register(b0)
{
    matrix cModelViewProjection;
    matrix cModel;
    matrix cModelInverse;
    float4 cCameraPosition;
    
    matrix cTextureTransform;
}

// Directional light constant buffer
cbuffer DirectionalLightBuffer : register(b1)
{
    float4 cLightDirection;
}

// Texture sampler
SamplerState gTextureSampler : register(s0);

// Textures
Texture2D gTextureDiffuse : register(t0);
Texture2D gTextureNormal : register(t1);