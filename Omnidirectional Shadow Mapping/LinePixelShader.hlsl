#include "LineShaderData.hlsli"

// Entry point for the vertex shader - will be executed for each pixel
float4 main(PixelInput input) : SV_TARGET
{
    return float4(input.colour, 1.0f);
}