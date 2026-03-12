#include "ShaderData.hlsli"

float CalculateShadowFactor(float3 position)
{
	// Calculate distance to light
    float3 depthToLightVector = position - cLightPosition.xyz;
    float depthToLight = length(depthToLightVector);

	// Sample
    float far_plane = 100.0f;
    float shadowDepth = gShadowMapTexture.Sample(gShadowSampler1, depthToLightVector / far_plane).r;

    float bias = 0.0005f;
    if (shadowDepth < (depthToLight / far_plane) - bias)
    {
        return 0.0f;
    }
    else
    {
        return 1.0f;
    }
}

//float CalculateShadowFactor(float3 position)
//{
//	// Calculate distance to light
//    float3 depthToLightVector = position - cLightPosition.xyz;
//	float depthToLight = length(depthToLightVector);

//	// Sample
//	float far_plane = 100.0f;
    
//    float compare = (depthToLight / far_plane);
//    float shadowDepth = gShadowMapTexture.Sample(gShadowSampler1, compare).r;

//    float bias = 0.0005f;
//    if (shadowDepth < compare)
//    { 
//        return 0.0f;
//    }
//    else
//    {
//        return 1.0f;
//    }
//}

float4 CalculatePointLighting(float3 position, float3 normal)
{
    float4 diffuse_light_colour = float4(0.8f, 0.8f, 0.8f, 1.0f);
    float4 ambient_light_colour = float4(0.2f, 0.2f, 0.2f, 1.0f);
    float4 specular_light_colour = float4(0.4f, 0.4f, 0.4f, 1.0f);

    // Shadow
    float shadow_factor = CalculateShadowFactor(position);
    
    // Calculate distance from the point light source
    float light_distance = length(distance(cLightPosition.xyz, position));
    
    // Attenuate
    float3 attenuate_constants = float3(CLightAttenuate.constant, CLightAttenuate.linear_, CLightAttenuate.quadratic);
    float attenuate = 1.0f / dot(attenuate_constants, float3(1.0f, light_distance, light_distance * light_distance));
    
    // Diffuse lighting
    float3 light_vector = normalize(cLightPosition.xyz - position);
    float diffuse_factor = saturate(dot(light_vector, normal));
    float4 diffuse_light = diffuse_factor * diffuse_light_colour * attenuate * shadow_factor;

    // Ambient lighting
    float4 ambient_light = ambient_light_colour;

    // Specular lighting
    float4 specular_light = 0.0f;
    if (diffuse_factor > 0.0f)
    {
        float3 view_direction = normalize(cCameraPosition.xyz - position);
        float3 reflect_direction = reflect(-light_vector, normal);

        float specular_factor = pow(saturate(dot(view_direction, reflect_direction)), 16.0f);
        specular_light = specular_factor * specular_light_colour * attenuate * shadow_factor;
    }
    
    // Combine the lights
    return (diffuse_light + ambient_light + specular_light);
}

// Entry point for the vertex shader - will be executed for each pixel
float4 main(PixelInput input) : SV_TARGET
{
    // Interpolating normal can unnormalize it, so normalize it.
    input.normal = normalize(input.normal);

	// Calculate light
    float4 light_colour = CalculatePointLighting(input.position.xyz, input.normal);
    
    // Gammer correction
    light_colour = pow(light_colour, 1.0f / 2.2f);
    
    return light_colour;
}