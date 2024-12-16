#include "Shader.h"
#include "Renderer.h"

#include <Windows.h>
#include "CompiledPixelShader.hlsl.h"
#include "CompiledVertexShader.hlsl.h"

#include <Windows.h>

namespace
{
	struct ModelViewProjectionBuffer
	{
		DirectX::XMMATRIX modelViewProjection;
		DirectX::XMMATRIX model;
	};

	__declspec(align(16))
	struct FogBuffer
	{
		DirectX::XMFLOAT3 camera_eye;
		float fog_start;
		float fog_range;
	};
}

Shader::Shader(Renderer* renderer) : m_Renderer(renderer)
{
}

void Shader::Load()
{
	this->LoadVertexShader();
	this->LoadPixelShader();
	this->CreateWorldViewProjectionConstantBuffer();
	this->CreateFogConstantBuffer();
}

void Shader::Use()
{
	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();

	// Bind the input layout to the pipeline's Input Assembler stage
	context->IASetInputLayout(m_VertexLayout.Get());

	// Bind the vertex shader to the pipeline's Vertex Shader stage
	context->VSSetShader(m_VertexShader.Get(), nullptr, 0);

	// Bind the pixel shader to the pipeline's Pixel Shader stage
	context->PSSetShader(m_PixelShader.Get(), nullptr, 0);

	// Bind the world constant buffer to the vertex shader
	const int constant_buffer_slot = 0;
	context->VSSetConstantBuffers(constant_buffer_slot, 1, m_ModelViewProjectionConstantBuffer.GetAddressOf());

	// Bind the fog constant buffer to the pixel shader
	const int fog_constant_buffer_slot = 1;
	context->PSSetConstantBuffers(fog_constant_buffer_slot, 1, m_FogConstantBuffer.GetAddressOf());
}

void Shader::LoadVertexShader()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Create the vertex shader
	DX::Check(device->CreateVertexShader(g_VertexShader, sizeof(g_VertexShader), nullptr, m_VertexShader.ReleaseAndGetAddressOf()));

	// Describe the memory layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT number_elements = ARRAYSIZE(layout);
	DX::Check(device->CreateInputLayout(layout, number_elements, g_VertexShader, sizeof(g_VertexShader), m_VertexLayout.ReleaseAndGetAddressOf()));
}

void Shader::LoadPixelShader()
{
	ID3D11Device* device = m_Renderer->GetDevice();
	device->CreatePixelShader(g_PixelShader, sizeof(g_PixelShader), nullptr, m_PixelShader.ReleaseAndGetAddressOf());
}

void Shader::CreateWorldViewProjectionConstantBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Create world constant buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ModelViewProjectionBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	DX::Check(device->CreateBuffer(&bd, nullptr, m_ModelViewProjectionConstantBuffer.ReleaseAndGetAddressOf()));
}

void Shader::CreateFogConstantBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Create world constant buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(FogBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	DX::Check(device->CreateBuffer(&bd, nullptr, m_FogConstantBuffer.ReleaseAndGetAddressOf()));
}

void Shader::UpdateModelViewProjectionBuffer(const DirectX::XMMATRIX& matrix, const DirectX::XMMATRIX& world)
{
	ModelViewProjectionBuffer buffer = {};
	buffer.modelViewProjection = DirectX::XMMatrixTranspose(matrix);
	buffer.model = DirectX::XMMatrixTranspose(world);

	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();
	context->UpdateSubresource(m_ModelViewProjectionConstantBuffer.Get(), 0, nullptr, &buffer, 0, 0);
}

void Shader::UpdateFogBuffer(const DirectX::XMFLOAT3& camera_eye, float fog_start, float fog_range)
{
	FogBuffer buffer = {};
	buffer.camera_eye = camera_eye;
	buffer.fog_start = fog_start;
	buffer.fog_range = fog_range;

	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();
	context->UpdateSubresource(m_FogConstantBuffer.Get(), 0, nullptr, &buffer, 0, 0);
}