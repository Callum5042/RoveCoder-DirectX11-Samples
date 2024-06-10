#include "RasterState.h"
#include "Renderer.h"

RasterState::RasterState(Renderer* renderer) : m_Renderer(renderer)
{
	CreateRasterStateSolid();
	CreateRasterStateWireframe();
}

void RasterState::Use()
{
	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();

	if (m_RenderWireframe)
	{
		context->RSSetState(m_RasterStateWireframe.Get());
	}
	else
	{
		context->RSSetState(m_RasterStateSolid.Get());
	}
}

void RasterState::ToggleWireframe()
{
	m_RenderWireframe = !m_RenderWireframe;
}

void RasterState::CreateRasterStateSolid()
{
	D3D11_RASTERIZER_DESC rasterizerState = {};
	rasterizerState.AntialiasedLineEnable = true;
	rasterizerState.CullMode = D3D11_CULL_FRONT;
	rasterizerState.FillMode = D3D11_FILL_SOLID;
	rasterizerState.DepthClipEnable = true;
	rasterizerState.FrontCounterClockwise = true;
	rasterizerState.MultisampleEnable = true;

	rasterizerState.DepthBias = 0;
	rasterizerState.DepthBiasClamp = 1.0f;
	rasterizerState.SlopeScaledDepthBias = 1.0f;

	ID3D11Device* device = m_Renderer->GetDevice();
	DX::Check(device->CreateRasterizerState(&rasterizerState, m_RasterStateSolid.ReleaseAndGetAddressOf()));
}

void RasterState::CreateRasterStateWireframe()
{
	D3D11_RASTERIZER_DESC rasterizerState = {};
	rasterizerState.AntialiasedLineEnable = true;
	rasterizerState.CullMode = D3D11_CULL_FRONT;
	rasterizerState.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerState.DepthClipEnable = true;
	rasterizerState.FrontCounterClockwise = true;
	rasterizerState.MultisampleEnable = true;

	rasterizerState.DepthBias = 0;
	rasterizerState.DepthBiasClamp = 1.0f;
	rasterizerState.SlopeScaledDepthBias = 1.0f;

	ID3D11Device* device = m_Renderer->GetDevice();
	DX::Check(device->CreateRasterizerState(&rasterizerState, m_RasterStateWireframe.ReleaseAndGetAddressOf()));
}