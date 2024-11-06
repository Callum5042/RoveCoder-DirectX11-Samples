#include "ShaderData.hlsli"

// Entry point for the vertex shader - will be executed for each vertex
PixelInput main(VertexInput input)
{
    PixelInput pixel_input;

	// Transform to homogeneous clip space
    pixel_input.positionClipSpace = mul(float4(input.position, 1.0f), cModelViewProjection);

    // Transform to world space
    pixel_input.position = input.position;
    
    // Pass the texture's UV coordinates
    pixel_input.tex_coord = mul(float4(input.tex_coord, 0.0f, 1.0f), cTextureTransform).xy;
    
    // Transform the normals and tangents by the inverse world space
    pixel_input.normal = mul(input.normal, (float3x3) cModelInverse).xyz;
    pixel_input.tangent = mul(input.tangent, (float3x3) cModel);
    
    return pixel_input;
}