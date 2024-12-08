#include "Plane.h"
#include "Renderer.h"
#include "Vertex.h"
#include "../External/WICTextureLoader.h"
#include <vector>
#include <string>
#include <filesystem>

Plane::Plane(Renderer* renderer) : m_Renderer(renderer)
{
}

void Plane::Create()
{
	CreateVertexBuffer();
	CreateIndexBuffer();
	LoadTexture();
}

void Plane::CreateVertexBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	const float width = 1.0f;
	const float height = 1.0f;
	const float depth = 1.0f;

	// Vertex data
	std::vector<Vertex> vertices =
	{
		{ VertexPosition(-width, -height, 0.0f), VertexTextureUV(0.0f, 1.0f) },
		{ VertexPosition(-width, +height, 0.0f), VertexTextureUV(0.0f, 0.0f) },
		{ VertexPosition(+width, +height, 0.0f), VertexTextureUV(1.0f, 0.0f) },
		{ VertexPosition(+width, -height, 0.0f), VertexTextureUV(1.0f, 1.0f) },
	};

	// Create vertex buffer
	D3D11_BUFFER_DESC vertexbuffer_desc = {};
	vertexbuffer_desc.Usage = D3D11_USAGE_DEFAULT;
	vertexbuffer_desc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertices.size());
	vertexbuffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertex_subdata = {};
	vertex_subdata.pSysMem = vertices.data();

	DX::Check(device->CreateBuffer(&vertexbuffer_desc, &vertex_subdata, m_VertexBuffer.ReleaseAndGetAddressOf()));
}

void Plane::CreateIndexBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Set Indices
	std::vector<UINT> indices =
	{
		0, 1, 2,
		0, 2, 3,
	};

	m_IndexCount = static_cast<UINT>(indices.size());

	// Create index buffer
	D3D11_BUFFER_DESC index_buffer_desc = {};
	index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	index_buffer_desc.ByteWidth = static_cast<UINT>(sizeof(UINT) * indices.size());
	index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA index_subdata = {};
	index_subdata.pSysMem = indices.data();

	DX::Check(device->CreateBuffer(&index_buffer_desc, &index_subdata, m_IndexBuffer.ReleaseAndGetAddressOf()));
}

void Plane::LoadTexture()
{
	std::wstring path = L"Wood_Crate_001_basecolor.png";

	// Check if file exists
	if (!std::filesystem::exists(path))
	{
		std::wstring error = L"Could not load file: " + path;
		MessageBox(NULL, error.c_str(), L"Error", MB_OK);
		return;
	}

	// Load texture into a resource shader view
	ID3D11Device* device = m_Renderer->GetDevice();
	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();

	ComPtr<ID3D11Resource> resource = nullptr;
	DX::Check(DirectX::CreateWICTextureFromFile(device, context, path.c_str(), resource.ReleaseAndGetAddressOf(), m_DiffuseTexture.ReleaseAndGetAddressOf()));
}

void Plane::Render()
{
	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();

	// We need to define the stride and offset
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// Bind the vertex buffer to the pipeline's Input Assembler stage
	context->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &stride, &offset);

	// Bind the index buffer to the pipeline's Input Assembler stage
	context->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Bind the geometry topology to the pipeline's Input Assembler stage
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Bind texture to the pixel shader
	context->PSSetShaderResources(0, 1, m_DiffuseTexture.GetAddressOf());

	// Render geometry
	context->DrawIndexed(m_IndexCount, 0, 0);
}

void Plane::SetTexture(ID3D11ShaderResourceView* texture)
{
	m_DiffuseTexture = texture;
}