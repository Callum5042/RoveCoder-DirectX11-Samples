// Vertex input
struct VertexInput
{
    float3 position : POSITION;
    float2 size : SIZE;
};

// Vertex output / Geometry input
struct GeometryInput
{
    float3 position : POSITION;
    float2 size : SIZE;
};

// Geometry output / Pixel input structure
struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texture_coord : TEXTURE;
};

// World constant buffer
cbuffer WorldBuffer : register(b0)
{
    matrix cWorld;
    matrix cView;
    matrix cProjection;
    float3 cCameraPosition;
    float padding;
}

// Texture animation buffer
cbuffer AnimationBuffer : register(b1)
{
    int cFrame;
}

// Texture sampler
SamplerState gSampler : register(s0);

// Diffuse texture
Texture2DArray gTextureSprite : register(t0);