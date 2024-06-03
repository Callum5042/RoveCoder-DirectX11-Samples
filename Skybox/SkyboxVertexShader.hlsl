#include "SkyboxShaderData.hlsli"

// Entry point for the vertex shader - will be executed for each vertex
PixelInput main(VertexInput input)
{
    PixelInput pixel_input;

	// Transform to homogeneous clip space
    pixel_input.positionClipSpace = mul(float4(input.position, 1.0f), cModelViewProjection);

    // Use local vertex position as cubemap lookup vector.
    pixel_input.position = input.position;

    return pixel_input;
}