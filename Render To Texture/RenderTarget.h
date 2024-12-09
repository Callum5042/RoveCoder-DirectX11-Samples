#pragma once

#include "Renderer.h"

class RenderTarget
{
	Renderer* m_Renderer = nullptr;
	ID3D11Device* m_Device = nullptr;
	ID3D11DeviceContext* m_DeviceContext = nullptr;

public:
	RenderTarget(Renderer* renderer);
	virtual ~RenderTarget() = default;

	void Create(int width, int height);
	void Use();

	ID3D11ShaderResourceView* GetTexture() const { return m_TextureShaderResource.Get(); }

private:

	// Render texture
	ComPtr<ID3D11Texture2D> m_Texture = nullptr;
	void CreateRenderTexture(int width, int height);

	// Render target and depth stencil
	ComPtr<ID3D11RenderTargetView> m_TextureRenderTargetView = nullptr;
	ComPtr<ID3D11DepthStencilView> m_TextureDepthStencilView = nullptr;
	void CreateRenderTargetAndDepthStencilView(int width, int height);

	// Shader resource view
	ComPtr<ID3D11ShaderResourceView> m_TextureShaderResource = nullptr;
	void CreateShaderResource();
};