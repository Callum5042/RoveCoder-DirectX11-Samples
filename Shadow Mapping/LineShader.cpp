#include "LineShader.h"
#include "Renderer.h"

#include <Windows.h>

#include "CompiledLinePixelShader.hlsl.h"
#include "CompiledLineVertexShader.hlsl.h"

#include <Windows.h>
#include <DirectXMath.h>

namespace
{
	struct ModelBuffer
	{
		XMMATRIX model;
	};

	struct CameraBuffer
	{
		XMMATRIX view;
		XMMATRIX projection;
		XMFLOAT4 position;
	};
}

LineShader::LineShader(Renderer* renderer) : m_Renderer(renderer)
{
}

void LineShader::Load()
{
	this->LoadVertexShader();
	this->LoadPixelShader();

	this->CreateModelConstantBuffer();
	this->CreateCameraConstantBuffer();
}

void LineShader::Use()
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
	context->VSSetConstantBuffers(constant_buffer_slot, 1, m_ModelConstantBuffer.GetAddressOf());
	context->PSSetConstantBuffers(constant_buffer_slot, 1, m_ModelConstantBuffer.GetAddressOf());

	// Bind the camera constant buffer to the vertex and pixel shader
	const int camera_buffer_slot = 1;
	context->VSSetConstantBuffers(camera_buffer_slot, 1, m_CameraConstantBuffer.GetAddressOf());
	context->PSSetConstantBuffers(camera_buffer_slot, 1, m_CameraConstantBuffer.GetAddressOf());
}

void LineShader::LoadVertexShader()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Create the vertex shader
	DX::Check(device->CreateVertexShader(g_LineVertexShader, sizeof(g_LineVertexShader), nullptr, m_VertexShader.ReleaseAndGetAddressOf()));

	// Describe the memory layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOUR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT number_elements = ARRAYSIZE(layout);
	DX::Check(device->CreateInputLayout(layout, number_elements, g_LineVertexShader, sizeof(g_LineVertexShader), m_VertexLayout.ReleaseAndGetAddressOf()));
}

void LineShader::LoadPixelShader()
{
	ID3D11Device* device = m_Renderer->GetDevice();
	device->CreatePixelShader(g_LinePixelShader, sizeof(g_LinePixelShader), nullptr, m_PixelShader.ReleaseAndGetAddressOf());
}

void LineShader::CreateModelConstantBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Create world constant buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ModelBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	DX::Check(device->CreateBuffer(&bd, nullptr, m_ModelConstantBuffer.ReleaseAndGetAddressOf()));
}

void LineShader::CreateCameraConstantBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Create world constant buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CameraBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	DX::Check(device->CreateBuffer(&bd, nullptr, m_CameraConstantBuffer.ReleaseAndGetAddressOf()));
}

void LineShader::UpdateModelBuffer(const XMMATRIX& transform)
{
	ModelBuffer buffer = {};
	buffer.model = XMMatrixTranspose(transform);

	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();
	context->UpdateSubresource(m_ModelConstantBuffer.Get(), 0, nullptr, &buffer, 0, 0);
}

void LineShader::UpdateCameraBuffer(const XMMATRIX& view, const XMMATRIX& projection, const XMFLOAT3& position)
{
	CameraBuffer buffer = {};
	buffer.projection = XMMatrixTranspose(projection);
	buffer.view = XMMatrixTranspose(view);
	buffer.position = XMFLOAT4(position.x, position.y, position.z, 1.0f);

	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();
	context->UpdateSubresource(m_CameraConstantBuffer.Get(), 0, nullptr, &buffer, 0, 0);
}
