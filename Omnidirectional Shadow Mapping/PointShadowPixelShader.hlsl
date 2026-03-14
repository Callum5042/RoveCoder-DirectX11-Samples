#include "ShaderData.hlsli"

// Entry point for the vertex shader - will be executed for each pixel
float main(PixelInput input) : SV_DEPTH
{
    float3 light_vector = input.position - cLightPosition.xyz;
    float lightVecLength = length(light_vector);

    float far_plane = 300.0f;

    return lightVecLength / far_plane;
}