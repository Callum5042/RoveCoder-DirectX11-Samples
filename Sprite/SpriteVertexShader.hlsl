#include "SpriteShaderData.hlsli"

// Entry point for the vertex shader - will be executed for each vertex
GeometryInput main(VertexInput input)
{
    GeometryInput output;

	// Pass through the vertex shader
    output.position = input.position;
    output.size = input.size;

    return output;
}