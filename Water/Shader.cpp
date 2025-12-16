#include "Shader.h"
#include "Renderer.h"

#include <Windows.h>
#include <d3dcompiler.h>

#include "CompiledPixelShader.hlsl.h"
#include "CompiledVertexShader.hlsl.h"

namespace
{
	struct ModelViewProjectionBuffer
	{
		DirectX::XMMATRIX modelViewProjection;
		DirectX::XMMATRIX modelInverse;
		DirectX::XMFLOAT4 cameraPosition;
		DirectX::XMFLOAT4 lightDirection;
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
	this->CreateTextureConstantBuffer();
	this->CreateWaveConstantBuffer();
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
	context->PSSetConstantBuffers(constant_buffer_slot, 1, m_ModelViewProjectionConstantBuffer.GetAddressOf());

	const int constant_buffer_slot1 = 1;
	context->VSSetConstantBuffers(constant_buffer_slot1, 1, m_TextureConstantBuffer.GetAddressOf());

	const int constant_buffer_slot2 = 2;
	context->VSSetConstantBuffers(constant_buffer_slot2, 1, m_WaveConstantBuffer.GetAddressOf());
}

void Shader::LoadVertexShader()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Compiler
	std::wstring path = L"D:/Sources/RoveCoder-DirectX11-Samples/Water/VertexShader.hlsl";
	ComPtr<ID3D10Blob> blob = CompileShader(path, ShaderType::Vertex);

	// Create the vertex shader
	DX::Check(device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_VertexShader.ReleaseAndGetAddressOf()));

	// Describe the memory layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT number_elements = ARRAYSIZE(layout);
	DX::Check(device->CreateInputLayout(layout, number_elements, blob->GetBufferPointer(), blob->GetBufferSize(), m_VertexLayout.ReleaseAndGetAddressOf()));
}

void Shader::LoadPixelShader()
{
	// Compiler
	std::wstring path = L"D:/Sources/RoveCoder-DirectX11-Samples/Water/PixelShader.hlsl";
	ComPtr<ID3D10Blob> blob = CompileShader(path, ShaderType::Pixel);

	// Create the pixel shader
	ID3D11Device* device = m_Renderer->GetDevice();
	device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_PixelShader.ReleaseAndGetAddressOf());
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

void Shader::CreateTextureConstantBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Create world constant buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(TextureBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	DX::Check(device->CreateBuffer(&bd, nullptr, m_TextureConstantBuffer.ReleaseAndGetAddressOf()));
}

void Shader::CreateWaveConstantBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Create world constant buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(GerstnerWaveBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	DX::Check(device->CreateBuffer(&bd, nullptr, m_WaveConstantBuffer.ReleaseAndGetAddressOf()));
}

ComPtr<ID3DBlob> Shader::CompileShader(const std::wstring& path, ShaderType shader_type)
{
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	std::string shader_target;
	if (shader_type == ShaderType::Vertex)
	{
		shader_target = "vs_5_0";
	}
	else if (shader_type == ShaderType::Pixel)
	{
		shader_target = "ps_5_0";
	}

	// Compiler from source file
	ComPtr<ID3DBlob> shader_blob = nullptr;
	ComPtr<ID3DBlob> error_blob = nullptr;
	HRESULT hr = D3DCompileFromFile(path.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", shader_target.c_str(), flags, 0, shader_blob.GetAddressOf(), error_blob.GetAddressOf());
	
	if (error_blob)
	{
		std::string error_message;
		error_message.assign(static_cast<const char*>(error_blob->GetBufferPointer()), error_blob->GetBufferSize());
	}

	DX::Check(hr);
	return shader_blob;
}

void Shader::UpdateModelViewProjectionBuffer(const DirectX::XMMATRIX& matrix, const DirectX::XMMATRIX& modelInverse, DirectX::XMFLOAT4 cameraPosition, DirectX::XMFLOAT4 lightDirection)
{
	ModelViewProjectionBuffer buffer = {};
	buffer.modelViewProjection = DirectX::XMMatrixTranspose(matrix);
	buffer.modelInverse = DirectX::XMMatrixTranspose(modelInverse);
	buffer.cameraPosition = cameraPosition;
	buffer.lightDirection = lightDirection;

	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();
	context->UpdateSubresource(m_ModelViewProjectionConstantBuffer.Get(), 0, nullptr, &buffer, 0, 0);
}

void Shader::UpdateTextureBuffer(const DirectX::XMMATRIX& matrix1, const DirectX::XMMATRIX& matrix2)
{
	TextureBuffer buffer = {};
	buffer.matrix1 = DirectX::XMMatrixTranspose(matrix1);
	buffer.matrix2 = DirectX::XMMatrixTranspose(matrix2);

	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();
	context->UpdateSubresource(m_TextureConstantBuffer.Get(), 0, nullptr, &buffer, 0, 0);
}

void Shader::UpdateWaveBuffer(GerstnerWaveBuffer buffer)
{
	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();
	context->UpdateSubresource(m_WaveConstantBuffer.Get(), 0, nullptr, &buffer, 0, 0);
}
