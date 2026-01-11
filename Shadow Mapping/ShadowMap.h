#pragma once

#include "Renderer.h"

const float SHADOW_MAP_SIZE = 4096.0f;

class ShadowMap
{
	Renderer* m_Renderer = nullptr;

public:
	ShadowMap(Renderer* renderer);
	virtual ~ShadowMap() = default;

	void Bind();

	inline ID3D11ShaderResourceView* GetShadowMapTexture() const
	{
		return m_ShadowMapTexture.Get();
	}

	inline ID3D11SamplerState* GetShadowSamplerState() const
	{
		return m_ShadowSampler.Get();
	}

private:
	const float m_ShadowMapTextureSize = SHADOW_MAP_SIZE;

	// Create resources
	void CreateShadowMapTexture();
	ComPtr<ID3D11ShaderResourceView> m_ShadowMapTexture = nullptr;
	ComPtr<ID3D11DepthStencilView> m_DepthStencilView = nullptr;

	// Raster state
	void CreateRasterModeBackCull();
	ComPtr<ID3D11RasterizerState> m_RasterModelBackShadow = nullptr;

	// Shadow map sampler
	void CreateShadowSampler();
	ComPtr<ID3D11SamplerState> m_ShadowSampler = nullptr;
};