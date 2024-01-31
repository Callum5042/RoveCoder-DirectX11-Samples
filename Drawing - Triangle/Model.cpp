#include "Model.h"
#include "Renderer.h"
#include "Vertex.h"
#include <vector>

Model::Model(Renderer* renderer) : m_Renderer(renderer)
{
}

void Model::Create()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Vertex data
	std::vector<Vertex> vertices =
	{
		{ +0.0f, +0.5f, 0.0f }, // Top vertex
		{ +0.5f, -0.5f, 0.0f }, // Right vertex
		{ -0.5f, -0.5f, 0.0f }  // Left vertex
	};

	m_VertexCount = static_cast<UINT>(vertices.size());

	// Create vertex buffer
	D3D11_BUFFER_DESC vertexbuffer_desc = {};
	vertexbuffer_desc.Usage = D3D11_USAGE_DEFAULT;
	vertexbuffer_desc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertices.size());
	vertexbuffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertex_subdata = {};
	vertex_subdata.pSysMem = vertices.data();

	DX::Check(device->CreateBuffer(&vertexbuffer_desc, &vertex_subdata, m_VertexBuffer.ReleaseAndGetAddressOf()));
}

void Model::Render()
{
	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();

	// We need the stride and offset
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// Bind the vertex buffer to the pipeline's Input Assembler stage
	context->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &stride, &offset);

	// Bind the geometry topology to the pipeline's Input Assembler stage
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Render geometry
	context->Draw(m_VertexCount, 0);
}