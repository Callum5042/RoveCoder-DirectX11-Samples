#include "SpriteShaderData.hlsli"

// Entry point for the vertex shader - will be executed for each pixel
float4 main(PixelInput input) : SV_TARGET
{
    float3 coord = float3(input.texture_coord.xy, cFrame);
    
    float4 sprite_texture = gTextureSprite.Sample(gSampler, coord);
    return sprite_texture;
}