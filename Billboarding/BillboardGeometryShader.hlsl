#include "BillboardShaderData.hlsli"

[maxvertexcount(4)]
void main(point GeometryInput input[1], inout TriangleStream<PixelInput> output)
{
	// Calculate vector perpendicular to the camera
    float3 plane_normal = input[0].position - cCameraPosition;
    plane_normal.y = 0.0f;
    plane_normal = normalize(plane_normal);

    float3 up_vector = float3(0.0f, 1.0f, 0.0f);
    float3 right_vector = normalize(cross(plane_normal, up_vector));

	// Shorten variables
    float3 position = input[0].position;
    float width = input[0].size.x;
    float height = input[0].size.y;

	// Create rectangle vertices that face the camera
    float3 vertices[4];
    vertices[0] = position + width * right_vector - height * up_vector;
    vertices[1] = position + width * right_vector + height * up_vector;
    vertices[2] = position - width * right_vector - height * up_vector;
    vertices[3] = position - width * right_vector + height * up_vector;

    // UV
    float2 uv[4];
    uv[0] = float2(0.0f, 1.0f);
    uv[1] = float2(0.0f, 0.0f);
    uv[2] = float2(1.0f, 1.0f);
    uv[3] = float2(1.0f, 0.0f);
    
	// Append output stream with our 4 new rectangle vertices
	[unroll]
    for (uint i = 0; i < 4; i++)
    {
        PixelInput element;

        element.position = mul(float4(vertices[i], 1.0f), cWorld);
        element.position = mul(element.position, cView);
        element.position = mul(element.position, cProjection);

        element.texture_coord = uv[i];
        
        output.Append(element);
    }
}