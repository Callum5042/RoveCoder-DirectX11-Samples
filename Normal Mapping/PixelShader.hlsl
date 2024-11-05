#include "ShaderData.hlsli"

float4 CalculateDirectionalLighting(float3 position, float3 normal)
{
    float4 diffuse_light_colour = float4(0.4f, 0.4f, 0.4f, 1.0f);
    float4 ambient_light_colour = float4(0.1f, 0.1f, 0.1f, 1.0f);
    float4 specular_light_colour = float4(0.6f, 0.6f, 0.6f, 1.0f);

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

        float specular_factor = pow(max(dot(view_direction, reflect_direction), 0.0), 32.0f);
        specular_light = float4(specular_factor * specular_light_colour);
    }

	// Combine the lights
    return diffuse_light + ambient_light + specular_light;
}

float3 CalculateNormalsFromNormalMap(float2 texture_uv, float3 normal, float3 tangent)
{
    float3 normalMapSample = gTextureNormal.Sample(gTextureSampler, texture_uv).rgb;

	// Uncompress each component from [0,1] to [-1,1].
    float3 normalT = normalize(normalMapSample * 2.0f - 1.0f);

    // Strength
    float3 normalStrength = 0.1f;
    normalT *= normalStrength + float3(0, 0, 1) * (1.0f - normalStrength);
    
	// Build orthonormal basis.
    float3 N = normal; // Normal
    float3 T = normalize(tangent - dot(tangent, N) * N); // Tangent
    float3 B = cross(N, T); // Bi-Tangent

    float3x3 TBN = float3x3(T, B, N);

	// Transform from tangent space to world space.
    float3 bumpedNormalW = mul(normalT, TBN);

    return normalize(bumpedNormalW);
}

// Entry point for the vertex shader - will be executed for each pixel
float4 main(PixelInput input) : SV_TARGET
{
    // Interpolating normal can unnormalize it, so normalize it.
    input.normal = normalize(input.normal);
    
    // Apply normal map
    input.normal = CalculateNormalsFromNormalMap(input.tex_coord, input.normal, input.tangent);
    
    // Diffuse texture
    float4 diffuse_texture = gTextureDiffuse.Sample(gTextureSampler, input.tex_coord);
    
	// Calculate directional light
    float4 light_colour = CalculateDirectionalLighting(input.position.xyz, input.normal);
    
    // Final colour
    float4 final_colour = diffuse_texture * light_colour;
    return final_colour;
}