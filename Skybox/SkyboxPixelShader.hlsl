#include "SkyboxShaderData.hlsli"

// Entry point for the vertex shader - will be executed for each pixel
float4 main(PixelInput input) : SV_TARGET
{
    float4 diffuse_texture = gTextureDiffuse.Sample(gTextureSampler, input.position);
    return diffuse_texture;
}