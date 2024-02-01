#include "ShaderData.hlsli"

// Entry point for the vertex shader - will be executed for each vertex
PixelInput main(VertexInput input)
{
    PixelInput pixel_input;

	// Transform to homogeneous clip space
    pixel_input.position = float4(input.position, 1.0f);

	// Set the vertex colour
    pixel_input.colour = input.colour;

    return pixel_input;
}