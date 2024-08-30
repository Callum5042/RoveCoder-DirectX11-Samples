#include "Model.h"
#include "Renderer.h"
#include "Vertex.h"
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
}

void Model::CreateVertexBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	const float width = 1.0f;
	const float height = 1.0f;
	const float depth = 1.0f;

	// Vertex data
	std::vector<Vertex> vertices =
	{
		{ VertexPosition(-width, -height, -depth), VertexNormal(+0.0f, +0.0f, -1.0f) },
		{ VertexPosition(-width, +height, -depth), VertexNormal(+0.0f, +0.0f, -1.0f) },
		{ VertexPosition(+width, +height, -depth), VertexNormal(+0.0f, +0.0f, -1.0f) },
		{ VertexPosition(+width, -height, -depth), VertexNormal(+0.0f, +0.0f, -1.0f) },

		{ VertexPosition(-width, -height, +depth), VertexNormal(+0.0f, +0.0f, +1.0f) },
		{ VertexPosition(+width, -height, +depth), VertexNormal(+0.0f, +0.0f, +1.0f) },
		{ VertexPosition(+width, +height, +depth), VertexNormal(+0.0f, +0.0f, +1.0f) },
		{ VertexPosition(-width, +height, +depth), VertexNormal(+0.0f, +0.0f, +1.0f) },
		
		{ VertexPosition(-width, +height, -depth), VertexNormal(+0.0f, +1.0f, +0.0f) },
		{ VertexPosition(-width, +height, +depth), VertexNormal(+0.0f, +1.0f, +0.0f) },
		{ VertexPosition(+width, +height, +depth), VertexNormal(+0.0f, +1.0f, +0.0f) },
		{ VertexPosition(+width, +height, -depth), VertexNormal(+0.0f, +1.0f, +0.0f) },

		{ VertexPosition(-width, -height, -depth), VertexNormal(+0.0f, -1.0f, +0.0f) },
		{ VertexPosition(+width, -height, -depth), VertexNormal(+0.0f, -1.0f, +0.0f) },
		{ VertexPosition(+width, -height, +depth), VertexNormal(+0.0f, -1.0f, +0.0f) },
		{ VertexPosition(-width, -height, +depth), VertexNormal(+0.0f, -1.0f, +0.0f) },

		{ VertexPosition(-width, -height, +depth), VertexNormal(-1.0f, +0.0f, +0.0f) },
		{ VertexPosition(-width, +height, +depth), VertexNormal(-1.0f, +0.0f, +0.0f) },
		{ VertexPosition(-width, +height, -depth), VertexNormal(-1.0f, +0.0f, +0.0f) },
		{ VertexPosition(-width, -height, -depth), VertexNormal(-1.0f, +0.0f, +0.0f) },

		{ VertexPosition(+width, -height, -depth), VertexNormal(+1.0f, +0.0f, +0.0f) },
		{ VertexPosition(+width, +height, -depth), VertexNormal(+1.0f, +0.0f, +0.0f) },
		{ VertexPosition(+width, +height, +depth), VertexNormal(+1.0f, +0.0f, +0.0f) },
		{ VertexPosition(+width, -height, +depth), VertexNormal(+1.0f, +0.0f, +0.0f) }
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