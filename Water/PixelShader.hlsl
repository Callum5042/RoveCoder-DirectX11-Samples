#include "ShaderData.hlsli"

float4 CalculateDirectionalLighting(float3 position, float3 normal)
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
    return diffuse_light + ambient_light + specular_light;
}

// Entry point for the vertex shader - will be executed for each pixel
float4 main(PixelInput input) : SV_TARGET
{
    // Calculate directional light
    float4 light_colour = CalculateDirectionalLighting(input.position.xyz, input.normal);

    light_colour = pow(light_colour, 1.0f / 2.2f);

    
    float4 diffuse_texture1 = gTextureDiffuse1.Sample(gTextureSampler, input.tex1);
    float4 diffuse_texture2 = gTextureDiffuse2.Sample(gTextureSampler, input.tex2);
    
    return light_colour * diffuse_texture1;// * diffuse_texture2;
    
    //float4 result = 1.0f - (1.0f - diffuse_texture1) * (1.0f - diffuse_texture2);
    // return light_colour; // * result;
    
    // return float4(input.normal, 1.0f);
    
    //float4 diffuse_light_colour = float4(0.6f, 0.6f, 0.6f, 1.0f);
    
    //float3 light_direction = -cLightDirection.xyz;
    //float diffuse_factor = saturate(dot(light_direction, input.normal));
    //float4 diffuse_light = diffuse_factor * diffuse_light_colour;

    //return float4(diffuse_light.xyz, 1.0f);
}