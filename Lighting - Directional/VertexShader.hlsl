#include "ShaderData.hlsli"

// Entry point for the vertex shader - will be executed for each vertex
PixelInput main(VertexInput input)
{
    PixelInput pixel_input;

	// Transform to homogeneous clip space
    pixel_input.position = mul(float4(input.position, 1.0f), cModelViewProjection);

	// Set the vertex colour
    pixel_input.tex = input.tex;
    
    // Set the normal
    pixel_input.normal = input.normal;

    return pixel_input;
}