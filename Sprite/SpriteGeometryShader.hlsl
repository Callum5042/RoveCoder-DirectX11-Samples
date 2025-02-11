#include "SpriteShaderData.hlsli"

[maxvertexcount(4)]
void main(point GeometryInput input[1], inout TriangleStream<PixelInput> output)
{
    // Get position and size of the sprite
    float3 position = input[0].position;
    float width = input[0].size.x;
    float height = input[0].size.y;

    // Calculate the vector from the sprite to the camera
    float3 to_camera = cCameraPosition - position;
    to_camera = normalize(to_camera);

    // Create the right and up vectors based on the direction to the camera
    float3 up_vector = float3(0.0f, 1.0f, 0.0f);

    // Calculate the right vector (perpendicular to both the up vector and the direction to the camera)
    float3 right_vector = normalize(cross(up_vector, to_camera));

    // Recalculate the up vector to ensure it remains perpendicular to both the direction and right vectors
    up_vector = normalize(cross(to_camera, right_vector));

    // Create the sprite's vertices that will face the camera
    float3 vertices[4];
    vertices[0] = position + right_vector * width - up_vector * height;
    vertices[1] = position + right_vector * width + up_vector * height;
    vertices[2] = position - right_vector * width - up_vector * height;
    vertices[3] = position - right_vector * width + up_vector * height;

    // UV coordinates
    float2 uv[4];
    uv[0] = float2(0.0f, 1.0f);
    uv[1] = float2(0.0f, 0.0f);
    uv[2] = float2(1.0f, 1.0f);
    uv[3] = float2(1.0f, 0.0f);

    // Append the new vertices to the output stream
    [unroll]
    for (uint i = 0; i < 4; i++)
    {
        PixelInput element;

        // Apply world, view, and projection transforms to each vertex
        element.position = mul(float4(vertices[i], 1.0f), cWorld);
        element.position = mul(element.position, cView);
        element.position = mul(element.position, cProjection);

        // Pass UV coordinates to the pixel shader
        element.texture_coord = uv[i];

        // Append the vertex to the stream
        output.Append(element);
    }
}
