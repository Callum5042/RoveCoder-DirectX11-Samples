#include "ShaderData.hlsli"

float4 CalculatePointLighting(float3 position, float3 normal)
{
    float4 diffuse_light_colour = float4(0.6f, 0.6f, 0.6f, 1.0f);
    float4 ambient_light_colour = float4(0.1f, 0.1f, 0.1f, 1.0f);
    float4 specular_light_colour = float4(0.2f, 0.2f, 0.2f, 1.0f);

    // Calculate distance from the point light source
    float light_distance = length(distance(cLightPosition.xyz, position));

    // Exit early if the light is beyond the max distance
    if (light_distance > cLightLength)
    {
        return ambient_light_colour;
    }

    // Smooth fade out for the light as it reaches the maximum distance
    float inner_radius = cLightLength * 0.8f; // Fade start distance
    float outer_radius = cLightLength; // Maximum light reach
    float attenuation = smoothstep(outer_radius, inner_radius, light_distance);

    // Diffuse lighting
    float3 light_vector = normalize(cLightPosition.xyz - position);
    float diffuse_factor = saturate(dot(light_vector, normal));
    float4 diffuse_light = diffuse_factor * diffuse_light_colour * attenuation;

    // Ambient lighting
    float4 ambient_light = ambient_light_colour;

    // Specular lighting
    float4 specular_light = 0.0f;
    if (diffuse_factor > 0.0f)
    {
        float3 view_direction = normalize(cCameraPosition.xyz - position);
        float3 reflect_direction = reflect(-light_vector, normal);

        float specular_factor = pow(saturate(dot(view_direction, reflect_direction)), 16.0f);
        specular_light = specular_factor * specular_light_colour * attenuation;
    }

    // Combine the lights
    return diffuse_light + ambient_light + specular_light;
}

// Entry point for the vertex shader - will be executed for each pixel
float4 main(PixelInput input) : SV_TARGET
{
    // Interpolating normal can unnormalize it, so normalize it.
    input.normal = normalize(input.normal);

	// Calculate point light
    float4 light_colour = CalculatePointLighting(input.position.xyz, input.normal);

    return light_colour;
}