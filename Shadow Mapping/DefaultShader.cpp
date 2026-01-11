#include "DefaultShader.h"
#include "Renderer.h"

#include <Windows.h>
#include "CompiledPixelShader.hlsl.h"
#include "CompiledVertexShader.hlsl.h"

#include <Windows.h>
#include <DirectXMath.h>

namespace
{
	struct ModelBuffer
	{
		XMMATRIX model;
		XMMATRIX model_inverse;
	};

	struct CameraBuffer
	{
		XMMATRIX view;
		XMMATRIX projection;
		XMFLOAT4 position;
	};

	struct DirectionalLightBuffer
	{
		XMFLOAT4 direction;
		XMMATRIX light_view;
		XMMATRIX light_projection;
	};
}

DefaultShader::DefaultShader(Renderer* renderer) : m_Renderer(renderer)
{
}

void DefaultShader::Load()
{
	this->LoadVertexShader();
	this->LoadPixelShader();

	this->CreateModelConstantBuffer();
	this->CreateCameraConstantBuffer();
	this->CreateDirectionalLightBuffer();
}

void DefaultShader::Use(bool bind_pixel_shader)
{
	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();

	// Bind the input layout to the pipeline's Input Assembler stage
	context->IASetInputLayout(m_VertexLayout.Get());

	// Bind the vertex shader to the pipeline's Vertex Shader stage
	context->VSSetShader(m_VertexShader.Get(), nullptr, 0);

	// Bind the pixel shader to the pipeline's Pixel Shader stage
	if (bind_pixel_shader)
	{
		context->PSSetShader(m_PixelShader.Get(), nullptr, 0);
	}
	else
	{
		context->PSSetShader(nullptr, nullptr, 0);
	}

	// Bind the world constant buffer to the vertex and pixel shader
	const int constant_buffer_slot = 0;
	context->VSSetConstantBuffers(constant_buffer_slot, 1, m_ModelConstantBuffer.GetAddressOf());
	context->PSSetConstantBuffers(constant_buffer_slot, 1, m_ModelConstantBuffer.GetAddressOf());

	// Bind the camera constant buffer to the vertex and pixel shader
	const int camera_buffer_slot = 1;
	context->VSSetConstantBuffers(camera_buffer_slot, 1, m_CameraConstantBuffer.GetAddressOf());
	context->PSSetConstantBuffers(camera_buffer_slot, 1, m_CameraConstantBuffer.GetAddressOf());

	// Bind the world constant buffer to the vertex and pixel shader
	const int light_buffer_slot = 2;
	context->VSSetConstantBuffers(light_buffer_slot, 1, m_DirectionalLightBuffer.GetAddressOf());
	context->PSSetConstantBuffers(light_buffer_slot, 1, m_DirectionalLightBuffer.GetAddressOf());
}

void DefaultShader::LoadVertexShader()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Create the vertex shader
	DX::Check(device->CreateVertexShader(g_VertexShader, sizeof(g_VertexShader), nullptr, m_VertexShader.ReleaseAndGetAddressOf()));

	// Describe the memory layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT number_elements = ARRAYSIZE(layout);
	DX::Check(device->CreateInputLayout(layout, number_elements, g_VertexShader, sizeof(g_VertexShader), m_VertexLayout.ReleaseAndGetAddressOf()));
}

void DefaultShader::LoadPixelShader()
{
	ID3D11Device* device = m_Renderer->GetDevice();
	device->CreatePixelShader(g_PixelShader, sizeof(g_PixelShader), nullptr, m_PixelShader.ReleaseAndGetAddressOf());
}

void DefaultShader::CreateModelConstantBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Create world constant buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ModelBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	DX::Check(device->CreateBuffer(&bd, nullptr, m_ModelConstantBuffer.ReleaseAndGetAddressOf()));
}

void DefaultShader::CreateCameraConstantBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Create world constant buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CameraBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	DX::Check(device->CreateBuffer(&bd, nullptr, m_CameraConstantBuffer.ReleaseAndGetAddressOf()));
}

void DefaultShader::UpdateModelBuffer(const XMMATRIX& transform)
{
	ModelBuffer buffer = {};
	buffer.model = XMMatrixTranspose(transform);
	buffer.model_inverse = XMMatrixTranspose(XMMatrixInverse(nullptr, transform));

	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();
	context->UpdateSubresource(m_ModelConstantBuffer.Get(), 0, nullptr, &buffer, 0, 0);
}

void DefaultShader::UpdateCameraBuffer(const XMMATRIX& view, const XMMATRIX& projection, const XMFLOAT3& position)
{
	CameraBuffer buffer = {};
	buffer.projection = XMMatrixTranspose(projection);
	buffer.view = XMMatrixTranspose(view);
	buffer.position = XMFLOAT4(position.x, position.y, position.z, 1.0f);

	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();
	context->UpdateSubresource(m_CameraConstantBuffer.Get(), 0, nullptr, &buffer, 0, 0);
}

void DefaultShader::CreateDirectionalLightBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Create world constant buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(DirectionalLightBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	DX::Check(device->CreateBuffer(&bd, nullptr, m_DirectionalLightBuffer.ReleaseAndGetAddressOf()));
}

void DefaultShader::UpdateDirectionalLightBuffer(const DirectX::XMFLOAT4& direction, const XMMATRIX& light_view, const XMMATRIX& light_projection)
{
	DirectionalLightBuffer buffer = {};
	buffer.direction = direction;
	buffer.light_view = XMMatrixTranspose(light_view);
	buffer.light_projection = XMMatrixTranspose(light_projection);

	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();
	context->UpdateSubresource(m_DirectionalLightBuffer.Get(), 0, nullptr, &buffer, 0, 0);
}