#include "ShaderData.hlsli"

// Entry point for the vertex shader - will be executed for each pixel
float4 main(PixelInput input) : SV_TARGET
{
    float4 diffuse_texture = gTextureDiffuse.Sample(gTextureSampler, input.tex);
    
    // Calculate the pixel's distance from the camera
    float pixel_distance = length(cCameraEyePosition - input.positionWorld);
    
    // Fog
    float4 fog_colour = float4(0.9f, 0.9f, 0.9f, 1.0f);
    float fog_factor = saturate((pixel_distance - cFogStart) / cFogRange);
    diffuse_texture = lerp(diffuse_texture, fog_colour, fog_factor);
    
    return diffuse_texture;
}