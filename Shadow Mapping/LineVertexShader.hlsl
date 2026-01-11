#include "LineShaderData.hlsli"

// Entry point for the vertex shader - will be executed for each vertex
PixelInput main(VertexInput input)
{
    PixelInput pixel_input;

	// Transform to homogeneous clip space
    pixel_input.positionClipSpace = mul(float4(input.position, 1.0f), cModelTransform);
    pixel_input.positionClipSpace = mul(pixel_input.positionClipSpace, cCameraView);
    pixel_input.positionClipSpace = mul(pixel_input.positionClipSpace, cCameraProjection);

    // Transform to world space.
    pixel_input.position = input.position;

    // Transform the normals by the inverse world space
    pixel_input.colour = input.colour;

    return pixel_input;
}