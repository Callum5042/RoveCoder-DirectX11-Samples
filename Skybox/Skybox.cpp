#include "Skybox.h"
#include "Renderer.h"
#include "Vertex.h"
#include "../External/DDSTextureLoader.h"
#include <vector>
#include <string>
#include <filesystem>

Skybox::Skybox(Renderer* renderer) : m_Renderer(renderer)
{
}

void Skybox::Create()
{
	CreateVertexBuffer();
	CreateIndexBuffer();
	LoadTexture();

	CreateRasterState();
	CreateDepthStencilState();
}

void Skybox::CreateVertexBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	const float width = 1.0f;
	const float height = 1.0f;
	const float depth = 1.0f;

	// Vertex data
	std::vector<SkyboxVertex> vertices =
	{
		{ VertexPosition(-width, -height, -depth) },
		{ VertexPosition(-width, +height, -depth) },
		{ VertexPosition(+width, +height, -depth) },
		{ VertexPosition(+width, -height, -depth) },

		{ VertexPosition(-width, -height, +depth) },
		{ VertexPosition(+width, -height, +depth) },
		{ VertexPosition(+width, +height, +depth) },
		{ VertexPosition(-width, +height, +depth) },

		{ VertexPosition(-width, +height, -depth) },
		{ VertexPosition(-width, +height, +depth) },
		{ VertexPosition(+width, +height, +depth) },
		{ VertexPosition(+width, +height, -depth) },

		{ VertexPosition(-width, -height, -depth) },
		{ VertexPosition(+width, -height, -depth) },
		{ VertexPosition(+width, -height, +depth) },
		{ VertexPosition(-width, -height, +depth) },

		{ VertexPosition(-width, -height, +depth) },
		{ VertexPosition(-width, +height, +depth) },
		{ VertexPosition(-width, +height, -depth) },
		{ VertexPosition(-width, -height, -depth) },

		{ VertexPosition(+width, -height, -depth) },
		{ VertexPosition(+width, +height, -depth) },
		{ VertexPosition(+width, +height, +depth) },
		{ VertexPosition(+width, -height, +depth) }
	};

	// Create vertex buffer
	D3D11_BUFFER_DESC vertexbuffer_desc = {};
	vertexbuffer_desc.Usage = D3D11_USAGE_DEFAULT;
	vertexbuffer_desc.ByteWidth = static_cast<UINT>(sizeof(SkyboxVertex) * vertices.size());
	vertexbuffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertex_subdata = {};
	vertex_subdata.pSysMem = vertices.data();

	DX::Check(device->CreateBuffer(&vertexbuffer_desc, &vertex_subdata, m_VertexBuffer.ReleaseAndGetAddressOf()));
}

void Skybox::CreateIndexBuffer()
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

void Skybox::LoadTexture()
{
	std::wstring path = L"cloudy_skybox.dds";

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
	DX::Check(DirectX::CreateDDSTextureFromFile(device, context, path.c_str(), resource.ReleaseAndGetAddressOf(), m_DiffuseTexture.ReleaseAndGetAddressOf()));
}

void Skybox::Render()
{
	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();

	// Apply raster state
	context->RSSetState(m_RasterState.Get());

	// Set depth stencil
	context->OMSetDepthStencilState(m_DepthStencilState.Get(), 0);

	// We need to define the stride and offset
	UINT stride = sizeof(SkyboxVertex);
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

void Skybox::CreateRasterState()
{
	D3D11_RASTERIZER_DESC rasterizerState = {};
	rasterizerState.CullMode = D3D11_CULL_BACK;
	rasterizerState.FillMode = D3D11_FILL_SOLID;
	rasterizerState.DepthClipEnable = true;
	rasterizerState.FrontCounterClockwise = true;

	ID3D11Device* device = m_Renderer->GetDevice();
	DX::Check(device->CreateRasterizerState(&rasterizerState, m_RasterState.ReleaseAndGetAddressOf()));
}

void Skybox::CreateDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
	depth_stencil_desc.DepthEnable = true;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	ID3D11Device* device = m_Renderer->GetDevice();
	device->CreateDepthStencilState(&depth_stencil_desc, &m_DepthStencilState);
}