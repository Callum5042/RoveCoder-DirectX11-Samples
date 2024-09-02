#include "ShaderData.hlsli"

float4 CalculateSpotLighting(float3 position, float3 normal)
{
    float4 diffuse_light_colour = float4(0.8f, 0.8f, 0.8f, 1.0f);
    float4 ambient_light_colour = float4(0.2f, 0.2f, 0.2f, 1.0f);
    float4 specular_light_colour = float4(0.4f, 0.4f, 0.4f, 1.0f);

    // Calculate distance from the point light source
    float light_distance = length(distance(cLightPosition.xyz, position));
        
    // Scale by spotlight factor and attenuate.
    float cone_size = cLightSpotCone;
    float3 direction = cLightDirection;
    
    float3 light_vector = normalize(cLightPosition.xyz - position);
    
    // Calculate spot light cone
    float spot = pow(max(dot(-light_vector, direction), 0.0f), cone_size);
    
    // Attenuate
    float3 attenuate_constants = float3(CLightAttenuate.constant, CLightAttenuate.linear_, CLightAttenuate.quadratic);
    float attenuate = spot / dot(attenuate_constants, float3(1.0f, light_distance, light_distance * light_distance));
    
    // Diffuse lighting
    float diffuse_factor = saturate(dot(light_vector, normal));
    float4 diffuse_light = diffuse_factor * diffuse_light_colour * attenuate;

    // Ambient lighting
    float4 ambient_light = ambient_light_colour * spot;

    // Specular lighting
    float4 specular_light = 0.0f;
    if (diffuse_factor > 0.0f)
    {
        float3 view_direction = normalize(cCameraPosition.xyz - position);
        float3 reflect_direction = reflect(-light_vector, normal);

        float specular_factor = pow(saturate(dot(view_direction, reflect_direction)), 16.0f);
        specular_light = specular_factor * specular_light_colour * attenuate;
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
    float4 light_colour = CalculateSpotLighting(input.position.xyz, input.normal);

    return light_colour;
}