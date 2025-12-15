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
	// Generate the geometry
	const float width = 50.0f;  // Grid width
	const float height = -1.0f;  // Grid height (fixed value for a flat plane)
	const float depth = 50.0f;  // Grid depth

	const int m = 500;  // Number of rows in the grid
	const int n = 500;  // Number of columns in the grid

	UINT vertexCount = m * n;
	UINT faceCount = (m - 1) * (n - 1) * 2;

	//
	// Create the vertices.
	//

	float halfWidth = 0.5f * width;
	float halfDepth = 0.5f * depth;

	float dx = width / (n - 1);
	float dz = depth / (m - 1);

	float du = 1.0f / (n - 1);
	float dv = 1.0f / (m - 1);

	m_VertexPosition.resize(vertexCount);
	m_VertexUV.resize(vertexCount);
	m_VertexNormal.resize(vertexCount);
	m_VertexTangent.resize(vertexCount);

	for (UINT i = 0; i < m; ++i)
	{
		float z = halfDepth - i * dz;
		for (UINT j = 0; j < n; ++j)
		{
			float x = -halfWidth + j * dx;

			m_VertexPosition[i * n + j] = VertexPosition(x, 0.0f, z);
			m_VertexNormal[i * n + j] = VertexNormal(0.0f, 1.0f, 0.0f);
			m_VertexTangent[i * n + j] = VertexTangent(1.0f, 0.0f, 0.0f);

			//// Stretch texture over grid.
			m_VertexUV[i * n + j].u = j * du;
			m_VertexUV[i * n + j].v = i * dv;
		}
	}

	//
	// Create the indices.
	//

	m_Indices.resize(faceCount * 3); // 3 indices per face

	// Iterate over each quad and compute indices.
	UINT k = 0;
	for (UINT i = 0; i < m - 1; ++i)
	{
		for (UINT j = 0; j < n - 1; ++j)
		{
			m_Indices[k] = i * n + j;
			m_Indices[k + 1] = i * n + j + 1;
			m_Indices[k + 2] = (i + 1) * n + j;

			m_Indices[k + 3] = (i + 1) * n + j;
			m_Indices[k + 4] = i * n + j + 1;
			m_Indices[k + 5] = (i + 1) * n + j + 1;

			k += 6; // next quad
		}
	}

	// Build D3D buffers
	CreateVertexPositionBuffer();
	CreateVertexUVBuffer();
	CreateVertexNormalBuffer();
	CreateVertexTangentBuffer();

	CreateIndexBuffer();
	LoadTexture();
}

void Model::CreateVertexPositionBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Create vertex buffer
	D3D11_BUFFER_DESC vertexbuffer_desc = {};
	vertexbuffer_desc.Usage = D3D11_USAGE_DEFAULT;
	vertexbuffer_desc.ByteWidth = static_cast<UINT>(sizeof(VertexPosition) * m_VertexPosition.size());
	vertexbuffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertex_subdata = {};
	vertex_subdata.pSysMem = m_VertexPosition.data();

	DX::Check(device->CreateBuffer(&vertexbuffer_desc, &vertex_subdata, m_VertexPositionBuffer.ReleaseAndGetAddressOf()));
}

void Model::CreateVertexUVBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Create vertex buffer
	D3D11_BUFFER_DESC vertexbuffer_desc = {};
	vertexbuffer_desc.Usage = D3D11_USAGE_DEFAULT;
	vertexbuffer_desc.ByteWidth = static_cast<UINT>(sizeof(VertexTextureUV) * m_VertexUV.size());
	vertexbuffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertex_subdata = {};
	vertex_subdata.pSysMem = m_VertexUV.data();

	DX::Check(device->CreateBuffer(&vertexbuffer_desc, &vertex_subdata, m_VertexUVBuffer.ReleaseAndGetAddressOf()));
}

void Model::CreateVertexNormalBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Create vertex buffer
	D3D11_BUFFER_DESC vertexbuffer_desc = {};
	vertexbuffer_desc.Usage = D3D11_USAGE_DEFAULT;
	vertexbuffer_desc.ByteWidth = static_cast<UINT>(sizeof(VertexNormal) * m_VertexNormal.size());
	vertexbuffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertex_subdata = {};
	vertex_subdata.pSysMem = m_VertexNormal.data();

	DX::Check(device->CreateBuffer(&vertexbuffer_desc, &vertex_subdata, m_VertexNormalBuffer.ReleaseAndGetAddressOf()));
}

void Model::CreateVertexTangentBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Create vertex buffer
	D3D11_BUFFER_DESC vertexbuffer_desc = {};
	vertexbuffer_desc.Usage = D3D11_USAGE_DEFAULT;
	vertexbuffer_desc.ByteWidth = static_cast<UINT>(sizeof(VertexTangent) * m_VertexTangent.size());
	vertexbuffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertex_subdata = {};
	vertex_subdata.pSysMem = m_VertexTangent.data();

	DX::Check(device->CreateBuffer(&vertexbuffer_desc, &vertex_subdata, m_VertexTangentBuffer.ReleaseAndGetAddressOf()));
}

void Model::CreateIndexBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Create index buffer
	D3D11_BUFFER_DESC index_buffer_desc = {};
	index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	index_buffer_desc.ByteWidth = static_cast<UINT>(sizeof(UINT) * m_Indices.size());
	index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA index_subdata = {};
	index_subdata.pSysMem = m_Indices.data();

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
	ID3D11Buffer* buffers[] = { m_VertexPositionBuffer.Get(), m_VertexUVBuffer.Get(), m_VertexNormalBuffer.Get(), m_VertexTangentBuffer.Get() };
	UINT strides[] = { sizeof(VertexPosition), sizeof(VertexTextureUV), sizeof(VertexNormal), sizeof(VertexTangent) };
	UINT offsets[] = { 0, 0, 0, 0 };

	context->IASetVertexBuffers(0, 4, buffers, strides, offsets);

	// Bind the index buffer to the pipeline's Input Assembler stage
	context->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Bind the geometry topology to the pipeline's Input Assembler stage
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Bind texture to the pixel shader
	ID3D11ShaderResourceView* srvs[] = { m_DiffuseTexture1.Get(), m_DiffuseTexture2.Get() };
	context->PSSetShaderResources(0, 2, srvs);

	// Render geometry
	context->DrawIndexed(m_Indices.size(), 0, 0);
}