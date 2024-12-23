#include "Shader.h"
#include "Renderer.h"

#include <Windows.h>
#include "CompiledPixelShader.hlsl.h"
#include "CompiledVertexShader.hlsl.h"

#include <Windows.h>
#include <DirectXMath.h>

namespace
{
	struct ModelViewProjectionBuffer
	{
		DirectX::XMMATRIX model_view_projection;
		DirectX::XMMATRIX model;
		DirectX::XMMATRIX model_inverse;
		DirectX::XMFLOAT3 camera_position;
		float padding;
	};

	struct Attenuation
	{
		float constant;
		float linear;
		float quadratic;
	};

	struct SpotLightBuffer
	{
		DirectX::XMFLOAT3 position;
		float padding1;
		Attenuation attenuation;
		float padding2;

		DirectX::XMFLOAT3 direction;
		float spot_cone;
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
	this->CreateSpotLightBuffer();
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

	// Bind the world constant buffer to the vertex and pixel shader
	const int constant_buffer_slot = 0;
	context->VSSetConstantBuffers(constant_buffer_slot, 1, m_ModelViewProjectionConstantBuffer.GetAddressOf());
	context->PSSetConstantBuffers(constant_buffer_slot, 1, m_ModelViewProjectionConstantBuffer.GetAddressOf());

	// Bind the world constant buffer to the vertex shader
	const int light_buffer_slot = 1;
	context->PSSetConstantBuffers(light_buffer_slot, 1, m_SpotLightBuffer.GetAddressOf());
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
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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

void Shader::UpdateModelViewProjectionBuffer(const DirectX::XMMATRIX& matrix, const DirectX::XMMATRIX& model, const DirectX::XMFLOAT3& cameraPosition)
{
	ModelViewProjectionBuffer buffer = {};
	buffer.model_view_projection = DirectX::XMMatrixTranspose(matrix);
	buffer.model = DirectX::XMMatrixTranspose(model);
	buffer.model_inverse = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, model));
	buffer.camera_position = cameraPosition;

	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();
	context->UpdateSubresource(m_ModelViewProjectionConstantBuffer.Get(), 0, nullptr, &buffer, 0, 0);
}

void Shader::CreateSpotLightBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Create world constant buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SpotLightBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	DX::Check(device->CreateBuffer(&bd, nullptr, m_SpotLightBuffer.ReleaseAndGetAddressOf()));
}

void Shader::UpdateSpotLightBuffer()
{   
	SpotLightBuffer buffer = {};
	buffer.position = DirectX::XMFLOAT3(0.0f, 300.0f, 200.0f);
	buffer.attenuation.constant = 1.0f;
	buffer.attenuation.linear = 0.0f;
	buffer.attenuation.quadratic = 0.01f;

	buffer.direction = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
	buffer.spot_cone = 96.0f;

	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();
	context->UpdateSubresource(m_SpotLightBuffer.Get(), 0, nullptr, &buffer, 0, 0);
}