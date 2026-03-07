#include "ShaderData.hlsli"

float4 CalculateDirectionalLighting(float3 position, float3 normal, float shadow_factor)
{
    float4 diffuse_light_colour = float4(0.6f, 0.6f, 0.6f, 1.0f);
    float4 ambient_light_colour = float4(0.1f, 0.1f, 0.1f, 1.0f);
    float4 specular_light_colour = float4(0.2f, 0.2f, 0.2f, 1.0f);

	// Light direction
    float3 light_direction = cLightDirection.xyz;

	// Diffuse lighting
    float diffuse_factor = saturate(dot(-light_direction, normal));
    float4 diffuse_light = diffuse_factor * diffuse_light_colour;

	// Ambient lighting
    float4 ambient_light = ambient_light_colour;

	// Specular lighting
    float4 specular_light = 0.0f;
    
    [flatten]
    if (diffuse_factor > 0.0f)
    {
        float3 view_direction = normalize(cCameraPosition.xyz - position);
        float3 reflect_direction = reflect(light_direction, normal);

        float specular_factor = pow(max(dot(view_direction, reflect_direction), 0.0), 16.0f);
        specular_light = float4(specular_factor * specular_light_colour);
    }

	// Combine the lights
    return ambient_light + ((diffuse_light + specular_light) * shadow_factor);
}

float CalculateShadowFactor(float4 light_view_projection)
{
    // Complete projection to NDC
    float3 shadow_coords = light_view_projection.xyz / light_view_projection.w;
    
    // Check if outside light frustum
    if (shadow_coords.z > 1.0f || shadow_coords.z < 0.0f)
    {
        return 1.0f;
    }

    // Transform from NDC [-1, 1] to UV [0, 1]
    float2 tex_coords;
    tex_coords.x = shadow_coords.x * 0.5f + 0.5f;
    tex_coords.y = -shadow_coords.y * 0.5f + 0.5f;

    // SampleCmp performs the comparison (pixel_depth < map_depth) 
    // and returns 1.0 if NOT in shadow, 0.0 if in shadow (based on D3D11_COMPARISON_LESS)
    return gShadowMap.SampleCmpLevelZero(gShadowSampler, tex_coords, shadow_coords.z);
}

// Entry point for the vertex shader - will be executed for each pixel
float4 main(PixelInput input) : SV_TARGET
{
    // Interpolating normal can unnormalize it, so normalize it.
    input.normal = normalize(input.normal);
    
    // Shadow
    float shadow_factor = CalculateShadowFactor(input.lightViewProjection);
    
	// Calculate directional light
    float4 light_colour = CalculateDirectionalLighting(input.position.xyz, input.normal, shadow_factor);
    
    // Gammer correction
    light_colour = pow(light_colour, 1.0f / 2.2f);
    
    return light_colour;
}