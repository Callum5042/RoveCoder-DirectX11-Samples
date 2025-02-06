#include "BillboardShader.h"
#include "Renderer.h"

#include <Windows.h>
#include "CompiledBillboardPixelShader.hlsl.h"
#include "CompiledBillboardVertexShader.hlsl.h"
#include "CompiledBillboardGeometryShader.hlsl.h"

SpriteShader::SpriteShader(Renderer* renderer) : m_Renderer(renderer)
{
}

void SpriteShader::Load()
{
	this->LoadVertexShader();
	this->LoadPixelShader();
	this->LoadGeometryShader();
	this->CreateWorldConstantBuffer();
}

void SpriteShader::Use()
{
	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();

	// Bind the input layout to the pipeline's Input Assembler stage
	context->IASetInputLayout(m_VertexLayout.Get());

	// Bind the vertex shader to the pipeline's Vertex Shader stage
	context->VSSetShader(m_VertexShader.Get(), nullptr, 0);

	// Bind the geometry shader to the pipeline's Geometry Shader stage
	context->GSSetShader(m_GeometryShader.Get(), nullptr, 0);

	// Bind the pixel shader to the pipeline's Pixel Shader stage
	context->PSSetShader(m_PixelShader.Get(), nullptr, 0);

	// Bind the world constant buffer to the vertex shader
	const int constant_buffer_slot = 0;
	context->VSSetConstantBuffers(constant_buffer_slot, 1, m_WorldConstantBuffer.GetAddressOf());

	// Bind the world constant buffer to the geometry shader
	context->GSSetConstantBuffers(0, 1, m_WorldConstantBuffer.GetAddressOf());
}

void SpriteShader::LoadVertexShader()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Create the vertex shader
	DX::Check(device->CreateVertexShader(g_BillboardVertexShader, sizeof(g_BillboardVertexShader), nullptr, m_VertexShader.ReleaseAndGetAddressOf()));

	// Describe the memory layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT number_elements = ARRAYSIZE(layout);
	DX::Check(device->CreateInputLayout(layout, number_elements, g_BillboardVertexShader, sizeof(g_BillboardVertexShader), m_VertexLayout.ReleaseAndGetAddressOf()));
}

void SpriteShader::LoadPixelShader()
{
	ID3D11Device* device = m_Renderer->GetDevice();
	device->CreatePixelShader(g_BillboardPixelShader, sizeof(g_BillboardPixelShader), nullptr, m_PixelShader.ReleaseAndGetAddressOf());
}

void SpriteShader::LoadGeometryShader()
{
	ID3D11Device* device = m_Renderer->GetDevice();
	DX::Check(device->CreateGeometryShader(g_BillboardGeometryShader, sizeof(g_BillboardGeometryShader), nullptr, m_GeometryShader.ReleaseAndGetAddressOf()));
}

void SpriteShader::CreateWorldConstantBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Create world constant buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WorldBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	DX::Check(device->CreateBuffer(&bd, nullptr, m_WorldConstantBuffer.ReleaseAndGetAddressOf()));
}

void SpriteShader::UpdateWorldConstantBuffer(const WorldBuffer& worldBuffer)
{
	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();
	context->UpdateSubresource(m_WorldConstantBuffer.Get(), 0, nullptr, &worldBuffer, 0, 0);
}