#include "Model.h"
#include "Renderer.h"
#include "Vertex.h"
#include "../External/WICTextureLoader.h"
#include <vector>
#include <string>
#include <filesystem>

Model::Model(Renderer* renderer) : m_Renderer(renderer)
{
}

void Model::Create()
{
	CreateVertexBuffer();
	CreateIndexBuffer();
	LoadTexture();
}

void Model::CreateVertexBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Vertex data
	std::vector<Vertex> vertices =
	{
		{ VertexPosition(-0.5f, +0.5f, 0.0f), VertexTextureUV(1.0f, 0.0f) }, // Top left vertex
		{ VertexPosition(+0.5f, +0.5f, 0.0f), VertexTextureUV(0.0f, 1.0f) }, // Top right vertex
		{ VertexPosition(+0.5f, -0.5f, 0.0f), VertexTextureUV(0.0f, 0.0f) }, // Bottom Right vertex
		{ VertexPosition(-0.5f, -0.5f, 0.0f), VertexTextureUV(1.0f, 1.0f) }, // Bottom left vertex
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

void Model::CreateIndexBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Set Indices
	std::vector<UINT> indices =
	{
		0, 1, 2, // Triangle 1
		2, 3, 0, // Triangle 2
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

void Model::LoadTexture()
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
	ComPtr<ID3D11Resource> resource = nullptr;
	DX::Check(DirectX::CreateWICTextureFromFile(device, path.c_str(), resource.ReleaseAndGetAddressOf(), m_DiffuseTexture.ReleaseAndGetAddressOf()));
}

void Model::Render()
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

	// Render geometry
	context->DrawIndexed(m_IndexCount, 0, 0);
}