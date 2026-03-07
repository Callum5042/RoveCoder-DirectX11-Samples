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

// Define a structure to hold the parameters for a single wave
struct GerstnerWave
{
    float Amplitude; // A (Height of the wave)
    float Wavelength; // Lambda (Used to calculate k)
    float Speed; // S (Used to calculate omega)
    float unused1;
    
    float2 Direction; // D (Normalized 2D vector [Dx, Dz])
    float Steepness; // Q (Controls the sharpness of the crests)
    float unused2;
};

// World constant buffer
cbuffer WorldBuffer : register(b0)
{
    matrix cModelViewProjection;
    matrix cModelInverse;
    float4 cCameraPosition;
    float4 cLightDirection;
}

// Texture
cbuffer TextureBuffer : register(b1)
{
    matrix cTextureMatrix1;
    matrix cTextureMatrix2;
}

// Global/Uniform variables (passed from the application/CPU)
cbuffer WaveParameters : register(b2)
{
    // Array to hold the parameters for N_WAVES (e.g., 4) waves
    GerstnerWave Waves[4];
    float Time;
};

// Texture sampler
SamplerState gTextureSampler : register(s0);

// Textures
Texture2D gTextureDiffuse1 : register(t0);
Texture2D gTextureDiffuse2 : register(t1);