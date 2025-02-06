#include "SpriteShaderData.hlsli"

// Entry point for the vertex shader - will be executed for each pixel
float4 main(PixelInput input) : SV_TARGET
{
    float4 sprite_texture = gTextureSprite.Sample(gSampler, input.texture_coord);
    return sprite_texture;
}