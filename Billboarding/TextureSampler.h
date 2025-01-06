#pragma once

#include <d3d11.h>

// This include is requires for using DirectX smart pointers (ComPtr)
#include <wrl\client.h>
using Microsoft::WRL::ComPtr;

class Renderer;

class TextureSampler
{
	Renderer* m_Renderer = nullptr;

public:
	TextureSampler(Renderer* renderer);
	virtual ~TextureSampler() = default;

	// Bind texture sampler to pipeline
	void Use();

private:
	ComPtr<ID3D11SamplerState> m_SamplerState;
	void CreateSamplerState();
};