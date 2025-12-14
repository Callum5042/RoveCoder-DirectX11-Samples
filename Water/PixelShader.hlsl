#include "ShaderData.hlsli"

// Entry point for the vertex shader - will be executed for each pixel
float4 main(PixelInput input) : SV_TARGET
{
    float4 diffuse_texture1 = gTextureDiffuse1.Sample(gTextureSampler, input.tex1);
    float4 diffuse_texture2 = gTextureDiffuse2.Sample(gTextureSampler, input.tex2);
    
    // return diffuse_texture1 * diffuse_texture2;
    
    float4 result = 1.0f - (1.0f - diffuse_texture1) * (1.0f - diffuse_texture2);
    return result;
}