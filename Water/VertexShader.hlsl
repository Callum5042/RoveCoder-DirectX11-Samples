#include "ShaderData.hlsli"

// Entry point for the vertex shader - will be executed for each vertex
PixelInput main(VertexInput input)
{
    PixelInput pixel_input;

	// Transform to homogeneous clip space
    pixel_input.position = mul(float4(input.position, 1.0f), cModelViewProjection);

	// Set the vertex colour
    pixel_input.tex1 = mul(float4(input.tex, 0.0f, 1.0f), cTextureMatrix1).xy;
    pixel_input.tex2 = mul(float4(input.tex, 0.0f, 1.0f), cTextureMatrix2).xy;

    return pixel_input;
}