#include "Billboard.h"
#include "Renderer.h"

#include <vector>

Billboard::Billboard(Renderer* renderer) : m_Renderer(renderer)
{
}

void Billboard::Create()
{
	CreateVertexBuffer();
}

void Billboard::CreateVertexBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Set vertex data
	m_Vertices =
	{
		{ -2.0f, 0.0f, 0.0f, 1.0f, 1.0f },
		{ +2.0f, 0.0f, 0.0f, 1.0f, 1.0f },
	};

	// Create index buffer
	D3D11_BUFFER_DESC vertex_buffer_desc = {};
	vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	vertex_buffer_desc.ByteWidth = static_cast<UINT>(sizeof(BillboardVertex) * m_Vertices.size());
	vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertex_subdata = {};
	vertex_subdata.pSysMem = m_Vertices.data();

	DX::Check(device->CreateBuffer(&vertex_buffer_desc, &vertex_subdata, m_VertexBuffer.ReleaseAndGetAddressOf()));
}

void Billboard::Render()
{
	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();

	// We need the stride and offset for the vertex
	UINT vertex_stride = sizeof(BillboardVertex);
	UINT vertex_offset = 0;

	// Bind the vertex buffer to the Input Assembler
	context->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &vertex_stride, &vertex_offset);

	// Bind the geometry topology to the Input Assembler
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// Render geometry
	context->Draw(static_cast<UINT>(m_Vertices.size()), 0);
}
