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
	CreateVertexPositionBuffer();
	CreateVertexUVBuffer();
	CreateIndexBuffer();
	LoadTexture();
}

void Model::CreateVertexPositionBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	const float width = 5.0f;
	const float height = -1.0f;
	const float depth = 5.0f;

	// Vertex data
	std::vector<VertexPosition> vertices =
	{
		{ VertexPosition(-width, +height, -depth) },
		{ VertexPosition(-width, +height, +depth) },
		{ VertexPosition(+width, +height, +depth) },
		{ VertexPosition(+width, +height, -depth) },
	};

	// Create vertex buffer
	D3D11_BUFFER_DESC vertexbuffer_desc = {};
	vertexbuffer_desc.Usage = D3D11_USAGE_DEFAULT;
	vertexbuffer_desc.ByteWidth = static_cast<UINT>(sizeof(VertexPosition) * vertices.size());
	vertexbuffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertex_subdata = {};
	vertex_subdata.pSysMem = vertices.data();

	DX::Check(device->CreateBuffer(&vertexbuffer_desc, &vertex_subdata, m_VertexPositionBuffer.ReleaseAndGetAddressOf()));
}

void Model::CreateVertexUVBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	const float width = 1.0f;
	const float height = 1.0f;
	const float depth = 1.0f;

	// Vertex data
	std::vector<VertexTextureUV> vertices =
	{
		{ VertexTextureUV(0.0f, 1.0f) },
		{ VertexTextureUV(0.0f, 0.0f) },
		{ VertexTextureUV(1.0f, 0.0f) },
		{ VertexTextureUV(1.0f, 1.0f) },
	};

	// Create vertex buffer
	D3D11_BUFFER_DESC vertexbuffer_desc = {};
	vertexbuffer_desc.Usage = D3D11_USAGE_DEFAULT;
	vertexbuffer_desc.ByteWidth = static_cast<UINT>(sizeof(VertexTextureUV) * vertices.size());
	vertexbuffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertex_subdata = {};
	vertex_subdata.pSysMem = vertices.data();

	DX::Check(device->CreateBuffer(&vertexbuffer_desc, &vertex_subdata, m_VertexUVBuffer.ReleaseAndGetAddressOf()));
}

void Model::CreateIndexBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Set Indices
	std::vector<UINT> indices =
	{
		0, 1, 2,
		0, 2, 3,
		4, 5, 6,
		4, 6, 7,
		8, 9, 10,
		8, 10, 11,
		12, 13, 14,
		12, 14, 15,
		16, 17, 18,
		16, 18, 19,
		20, 21, 22,
		20, 22, 23,
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
	std::wstring path = L"D:/3D models/Textures/water_base.png";

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
	DX::Check(DirectX::CreateWICTextureFromFile(device, context, path.c_str(), resource.ReleaseAndGetAddressOf(), m_DiffuseTexture1.ReleaseAndGetAddressOf()));

	path = L"D:/3D models/Textures/water_sheen.png";
	DX::Check(DirectX::CreateWICTextureFromFile(device, context, path.c_str(), resource.ReleaseAndGetAddressOf(), m_DiffuseTexture2.ReleaseAndGetAddressOf()));
}

void Model::Render()
{
	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();

	// Bind the vertex buffer to the pipeline's Input Assembler stage
	ID3D11Buffer* buffers[] = { m_VertexPositionBuffer.Get(), m_VertexUVBuffer.Get() };
	UINT strides[] = { sizeof(VertexPosition), sizeof(VertexTextureUV) };
	UINT offsets[] = { 0, 0 };

	context->IASetVertexBuffers(0, 2, buffers, strides, offsets);

	// Bind the index buffer to the pipeline's Input Assembler stage
	context->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Bind the geometry topology to the pipeline's Input Assembler stage
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Bind texture to the pixel shader
	context->PSSetShaderResources(0, 1, m_DiffuseTexture1.GetAddressOf());
	context->PSSetShaderResources(1, 1, m_DiffuseTexture2.GetAddressOf());

	// Render geometry
	context->DrawIndexed(m_IndexCount, 0, 0);
}