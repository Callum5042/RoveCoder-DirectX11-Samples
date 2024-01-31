// Entry point for the vertex shader - will be executed for each vertex
float4 main(float3 position : POSITION) : SV_POSITION
{
	// We simply pass our vertex onto the next stage for now
    return float4(position, 1.0f);
}