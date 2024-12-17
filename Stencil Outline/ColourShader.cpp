#include "ColourShader.h"
#include "Renderer.h"

#include <Windows.h>
#include "CompiledColourPixelShader.hlsl.h"
#include "CompiledColourVertexShader.hlsl.h"

#include <Windows.h>

namespace
{
	struct ModelViewProjectionBuffer
	{
		DirectX::XMMATRIX modelViewProjection;
	};
}

ColourShader::ColourShader(Renderer* renderer) : m_Renderer(renderer)
{
}

void ColourShader::Load()
{
	this->LoadVertexShader();
	this->LoadPixelShader();
	this->CreateWorldViewProjectionConstantBuffer();
}

void ColourShader::Use()
{
	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();

	// Bind the input layout to the pipeline's Input Assembler stage
	context->IASetInputLayout(m_VertexLayout.Get());

	// Bind the vertex ColourShader to the pipeline's Vertex ColourShader stage
	context->VSSetShader(m_VertexShader.Get(), nullptr, 0);

	// Bind the pixel ColourShader to the pipeline's Pixel ColourShader stage
	context->PSSetShader(m_PixelShader.Get(), nullptr, 0);

	// Bind the world constant buffer to the vertex ColourShader
	const int constant_buffer_slot = 0;
	context->VSSetConstantBuffers(constant_buffer_slot, 1, m_ModelViewProjectionConstantBuffer.GetAddressOf());
}

void ColourShader::LoadVertexShader()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Create the vertex ColourShader
	DX::Check(device->CreateVertexShader(g_ColourVertexShader, sizeof(g_ColourVertexShader), nullptr, m_VertexShader.ReleaseAndGetAddressOf()));

	// Describe the memory layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT number_elements = ARRAYSIZE(layout);
	DX::Check(device->CreateInputLayout(layout, number_elements, g_ColourVertexShader, sizeof(g_ColourVertexShader), m_VertexLayout.ReleaseAndGetAddressOf()));
}

void ColourShader::LoadPixelShader()
{
	ID3D11Device* device = m_Renderer->GetDevice();
	device->CreatePixelShader(g_ColourPixelShader, sizeof(g_ColourPixelShader), nullptr, m_PixelShader.ReleaseAndGetAddressOf());
}

void ColourShader::CreateWorldViewProjectionConstantBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Create world constant buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ModelViewProjectionBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	DX::Check(device->CreateBuffer(&bd, nullptr, m_ModelViewProjectionConstantBuffer.ReleaseAndGetAddressOf()));
}

void ColourShader::UpdateModelViewProjectionBuffer(const DirectX::XMMATRIX& matrix)
{
	ModelViewProjectionBuffer buffer = {};
	buffer.modelViewProjection = DirectX::XMMatrixTranspose(matrix);

	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();
	context->UpdateSubresource(m_ModelViewProjectionConstantBuffer.Get(), 0, nullptr, &buffer, 0, 0);
}