// Vertex input
struct VertexInput
{
    float3 position : POSITION;
    float4 colour : COLOUR;
};

// Pixel input structure
struct PixelInput
{
    float4 position : SV_POSITION;
    float4 colour : COLOUR;
};