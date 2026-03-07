#include "ShaderData.hlsli"

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
    pixel_input.normal = mul(input.normal, (float3x3) cModelTransformInverse).xyz;

    // Pass UV
    pixel_input.uv = input.uv;
    
    // Calculate light position - used to sample the shadow map
    pixel_input.lightViewProjection = mul(float4(input.position, 1.0f), cModelTransform);
    pixel_input.lightViewProjection = mul(pixel_input.lightViewProjection, cLightView);
    pixel_input.lightViewProjection = mul(pixel_input.lightViewProjection, cLightProjection);
    
    return pixel_input;
}