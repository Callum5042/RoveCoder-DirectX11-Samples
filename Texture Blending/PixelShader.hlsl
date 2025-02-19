#include "ShaderData.hlsli"

// Entry point for the vertex shader - will be executed for each pixel
float4 main(PixelInput input) : SV_TARGET
{
    // Sample textures
    float4 texture1 = gTextureDiffuse1.Sample(gTextureSampler, input.tex);
    float4 texture2 = gTextureDiffuse2.Sample(gTextureSampler, input.tex);
    float4 texture3 = gTextureDiffuse3.Sample(gTextureSampler, input.tex);

    const int interpolate = 1;
    const int screen = 2;
    
    if (padding.x == interpolate)
    {
        // Interpolate with mask
        float4 final_colour = lerp(texture1, texture2, texture3);
        return float4(final_colour.rgb, 1.0f);
    }
    else if (padding.x == screen)
    {
        // Screen
        float4 final_colour = 1.0 - (1.0 - texture1) * (1.0 - texture3);
        return float4(final_colour.rgb, 0.6f);
    }
    
    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}