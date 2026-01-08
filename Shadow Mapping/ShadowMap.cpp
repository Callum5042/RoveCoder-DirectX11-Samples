#include "ShadowMap.h"

ShadowMap::ShadowMap(Renderer* renderer) : m_Renderer(renderer)
{
	this->CreateShadowMapTexture();
	this->CreateShadowMapTexture();
	this->CreateRasterModeBackCull();
	this->CreateShadowSampler();
}

void ShadowMap::Bind()
{
	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();

	// Clear the render target view to the chosen colour
	context->ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Bind the render target view to the pipeline's output merger stage
	ID3D11RenderTargetView* render_target_views[1] = { nullptr };
	context->OMSetRenderTargets(1, render_target_views, m_DepthStencilView.Get());

	// Describe the viewport
	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(m_ShadowMapTextureSize);
	viewport.Height = static_cast<float>(m_ShadowMapTextureSize);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	// Bind viewport to the pipline's rasterization stage
	context->RSSetViewports(1, &viewport);

	// Set raster state
	context->RSSetState(m_RasterModelBackShadow.Get());
}

void ShadowMap::CreateShadowMapTexture()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Create shadow map texture
	D3D11_TEXTURE2D_DESC texture_desc = {};
	texture_desc.Width = static_cast<UINT>(m_ShadowMapTextureSize);
	texture_desc.Height = static_cast<UINT>(m_ShadowMapTextureSize);
	texture_desc.ArraySize = 1;
	texture_desc.SampleDesc.Count = 1;
	texture_desc.SampleDesc.Quality = 0;
	texture_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texture_desc.Usage = D3D11_USAGE_DEFAULT;
	texture_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	ComPtr<ID3D11Texture2D> texture = nullptr;
	DX::Check(device->CreateTexture2D(&texture_desc, 0, texture.GetAddressOf()));

	// Create depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC depth_view_desc = {};
	depth_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	DX::Check(device->CreateDepthStencilView(texture.Get(), &depth_view_desc, m_DepthStencilView.GetAddressOf()));

	// Create shadow resource
	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
	shader_resource_view_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shader_resource_view_desc.Texture2D.MipLevels = 1;

	DX::Check(device->CreateShaderResourceView(texture.Get(), &shader_resource_view_desc, m_ShadowMapTexture.GetAddressOf()));
}

void ShadowMap::CreateRasterModeBackCull()
{
	D3D11_RASTERIZER_DESC desc = {};
	desc.AntialiasedLineEnable = false;
	desc.CullMode = D3D11_CULL_FRONT;
	desc.FillMode = D3D11_FILL_SOLID;
	desc.DepthClipEnable = false;
	desc.FrontCounterClockwise = false;
	desc.MultisampleEnable = false;

	desc.DepthBias = 20;
	desc.DepthBiasClamp = 0.01f;
	desc.SlopeScaledDepthBias = 2.0f;

	ID3D11Device* device = m_Renderer->GetDevice();
	DX::Check(device->CreateRasterizerState(&desc, m_RasterModelBackShadow.ReleaseAndGetAddressOf()));
}

void ShadowMap::CreateShadowSampler()
{
	D3D11_SAMPLER_DESC desc = {};
	desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.BorderColor[0] = 1.0f;
	desc.BorderColor[1] = 1.0f;
	desc.BorderColor[2] = 1.0f;
	desc.BorderColor[3] = 1.0f;
	desc.MinLOD = 0.0f;
	desc.MaxLOD = D3D11_FLOAT32_MAX;
	desc.MipLODBias = 0.0f;
	desc.MaxAnisotropy = 0;
	desc.ComparisonFunc = D3D11_COMPARISON_LESS;
	desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;

	ID3D11Device* device = m_Renderer->GetDevice();
	DX::Check(device->CreateSamplerState(&desc, m_ShadowSampler.GetAddressOf()));
}
