#include "ShaderData.hlsli"

// Entry point for the vertex shader - will be executed for each vertex
PixelInput main(VertexInput input)
{
    PixelInput pixel_input;

	// Transform to homogeneous clip space
    pixel_input.positionWorld = mul(float4(input.position, 1.0f), cModelWorld).xyz;
    pixel_input.position = mul(float4(input.position, 1.0f), cModelViewProjection);

	// Set the vertex colour
    pixel_input.tex = input.tex;

    return pixel_input;
}