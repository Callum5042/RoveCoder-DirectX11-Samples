#include "RenderTarget.h"
#include <DirectXColors.h>

RenderTarget::RenderTarget(Renderer* renderer) : m_Renderer(renderer)
{
	m_Device = m_Renderer->GetDevice();
	m_DeviceContext = m_Renderer->GetDeviceContext();
}

void RenderTarget::Create(int width, int height)
{
	CreateRenderTexture(width, height);
	CreateRenderTargetAndDepthStencilView(width, height);
	CreateShaderResource();
}

void RenderTarget::Use()
{
	// Clear the render target view to the chosen colour
	m_DeviceContext->ClearRenderTargetView(m_TextureRenderTargetView.Get(), reinterpret_cast<const float*>(&DirectX::Colors::DarkGreen));
	m_DeviceContext->ClearDepthStencilView(m_TextureDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Bind the render target view to the pipeline's output merger stage
	m_DeviceContext->OMSetRenderTargets(1, m_TextureRenderTargetView.GetAddressOf(), m_TextureDepthStencilView.Get());
}

void RenderTarget::CreateRenderTexture(int width, int height)
{
	D3D11_TEXTURE2D_DESC texture_desc = {};
	texture_desc.Width = width;
	texture_desc.Height = height;
	texture_desc.MipLevels = 1;
	texture_desc.ArraySize = 1;
	texture_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texture_desc.SampleDesc.Count = 1;
	texture_desc.SampleDesc.Quality = 0;
	texture_desc.Usage = D3D11_USAGE_DEFAULT;
	texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texture_desc.CPUAccessFlags = 0;
	texture_desc.MiscFlags = 0;

	DX::Check(m_Device->CreateTexture2D(&texture_desc, 0, m_Texture.ReleaseAndGetAddressOf()));
}

void RenderTarget::CreateRenderTargetAndDepthStencilView(int width, int height)
{
	// Create the render target view.
	D3D11_RENDER_TARGET_VIEW_DESC target_view_desc = {};
	target_view_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	target_view_desc.Texture2D.MipSlice = 0;

	DX::Check(m_Device->CreateRenderTargetView(m_Texture.Get(), &target_view_desc, m_TextureRenderTargetView.ReleaseAndGetAddressOf()));

	// Depth stencil
	D3D11_TEXTURE2D_DESC depth_texture_desc = {};
	depth_texture_desc.Width = width;
	depth_texture_desc.Height = height;
	depth_texture_desc.MipLevels = 1;
	depth_texture_desc.ArraySize = 1;
	depth_texture_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_texture_desc.SampleDesc.Count = 1;
	depth_texture_desc.SampleDesc.Quality = 0;
	depth_texture_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_texture_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	ComPtr<ID3D11Texture2D> texture = nullptr;
	DX::Check(m_Device->CreateTexture2D(&depth_texture_desc, nullptr, texture.ReleaseAndGetAddressOf()));
	DX::Check(m_Device->CreateDepthStencilView(texture.Get(), nullptr, m_TextureDepthStencilView.ReleaseAndGetAddressOf()));
}

void RenderTarget::CreateShaderResource()
{
	D3D11_SHADER_RESOURCE_VIEW_DESC view_desc = {};
	view_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	view_desc.Texture2D.MostDetailedMip = 0;
	view_desc.Texture2D.MipLevels = 1;

	DX::Check(m_Device->CreateShaderResourceView(m_Texture.Get(), &view_desc, m_TextureShaderResource.ReleaseAndGetAddressOf()));
}