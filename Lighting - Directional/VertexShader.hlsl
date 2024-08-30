#include "ShaderData.hlsli"

// Entry point for the vertex shader - will be executed for each vertex
PixelInput main(VertexInput input)
{
    PixelInput pixel_input;

	// Transform to homogeneous clip space
    pixel_input.positionClipSpace = mul(float4(input.position, 1.0f), cModelViewProjection);

    // Transform to world space.
    pixel_input.position = input.position;

    // Transform the normals by the inverse world space
    pixel_input.normal = mul(input.normal, (float3x3) cModelInverse).xyz;

    return pixel_input;
}