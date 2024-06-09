#include "Model.h"
#include "Renderer.h"
#include <vector>
#include <string>
#include <sstream>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../External/TinyGLTF/tiny_gltf.h"

Model::Model(Renderer* renderer) : m_Renderer(renderer)
{
}

void Model::Create()
{
	// Load GLTF model
	tinygltf::TinyGLTF loader;
	tinygltf::Model model;
	std::string error;
	std::string warning;

	bool success = loader.LoadBinaryFromFile(&model, &error, &warning, "monkey.glb");
	if (!success)
	{
		std::wstringstream ss;
		ss << "Error: " << error.c_str();
		MessageBox(NULL, ss.str().c_str(), L"Error", MB_OK);
		return;
	}

	// Get the geometry data
	tinygltf::Scene scene = model.scenes[model.defaultScene];

	for (int i = 0; i < scene.nodes.size(); ++i)
	{
		 tinygltf::Node node = model.nodes[scene.nodes[i]];

		 const tinygltf::Mesh& mesh = model.meshes[node.mesh];
		 for (size_t i = 0; i < mesh.primitives.size(); ++i)
		 {
			 const tinygltf::Primitive& primitive = mesh.primitives[i];

			 if (primitive.attributes.find("POSITION") != primitive.attributes.end())
			 {
				 const tinygltf::Accessor& accessor = model.accessors[primitive.attributes.find("POSITION")->second];
				 const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
				 const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

				 const float* positions = reinterpret_cast<const float*>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));

				 for (size_t v = 0; v < accessor.count; ++v)
				 {
					 Vertex vertex;
					 vertex.position.x = positions[v * 3 + 0];
					 vertex.position.y = positions[v * 3 + 1];
					 vertex.position.z = positions[v * 3 + 2];
					 m_Vertices.push_back(vertex);
				 }
			 }

			 // Extract indices
			 if (primitive.indices >= 0)
			 {
				 const tinygltf::Accessor& accessor = model.accessors[primitive.indices];
				 const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
				 const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

				 if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
				 {
					 const uint8_t* buf = reinterpret_cast<const uint8_t*>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
					 for (size_t index = 0; index < accessor.count; index++)
					 {
						 m_Indices.push_back(static_cast<uint32_t>(buf[index]));
					 }
				 }
				 else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
				 {
					 const uint16_t* buf = reinterpret_cast<const uint16_t*>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
					 for (size_t index = 0; index < accessor.count; index++)
					 {
						 m_Indices.push_back(static_cast<uint32_t>(buf[index]));
					 }
				 }
				 else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
				 {
					 const uint32_t* buf = reinterpret_cast<const uint32_t*>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
					 for (size_t index = 0; index < accessor.count; index++)
					 {
						 m_Indices.push_back(buf[index]);
					 }
				 }
			 }
		 }
	}


	CreateVertexBuffer();
	CreateIndexBuffer();
}

void Model::CreateVertexBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Vertex data
	/*std::vector<Vertex> vertices =
	{
		{ VertexPosition(-1.0f, -1.0f, -1.0f), VertexColour(1.0f, 0.0f, 0.0f, 1.0f) },
		{ VertexPosition(-1.0f, +1.0f, -1.0f), VertexColour(1.0f, 0.0f, 0.0f, 1.0f) },
		{ VertexPosition(+1.0f, +1.0f, -1.0f), VertexColour(1.0f, 0.0f, 0.0f, 1.0f) },
		{ VertexPosition(+1.0f, -1.0f, -1.0f), VertexColour(1.0f, 0.0f, 0.0f, 1.0f) },
		{ VertexPosition(-1.0f, -1.0f, +1.0f), VertexColour(1.0f, 0.0f, 0.0f, 1.0f) },
		{ VertexPosition(+1.0f, -1.0f, +1.0f), VertexColour(1.0f, 0.0f, 0.0f, 1.0f) },
		{ VertexPosition(+1.0f, +1.0f, +1.0f), VertexColour(1.0f, 0.0f, 0.0f, 1.0f) },
		{ VertexPosition(-1.0f, +1.0f, +1.0f), VertexColour(1.0f, 0.0f, 0.0f, 1.0f) },
		{ VertexPosition(-1.0f, +1.0f, -1.0f), VertexColour(1.0f, 0.0f, 0.0f, 1.0f) },
		{ VertexPosition(-1.0f, +1.0f, +1.0f), VertexColour(1.0f, 0.0f, 0.0f, 1.0f) },
		{ VertexPosition(+1.0f, +1.0f, +1.0f), VertexColour(1.0f, 0.0f, 0.0f, 1.0f) },
		{ VertexPosition(+1.0f, +1.0f, -1.0f), VertexColour(1.0f, 0.0f, 0.0f, 1.0f) },
		{ VertexPosition(-1.0f, -1.0f, -1.0f), VertexColour(1.0f, 0.0f, 0.0f, 1.0f) },
		{ VertexPosition(+1.0f, -1.0f, -1.0f), VertexColour(1.0f, 0.0f, 0.0f, 1.0f) },
		{ VertexPosition(+1.0f, -1.0f, +1.0f), VertexColour(1.0f, 0.0f, 0.0f, 1.0f) },
		{ VertexPosition(-1.0f, -1.0f, +1.0f), VertexColour(1.0f, 0.0f, 0.0f, 1.0f) },
		{ VertexPosition(-1.0f, -1.0f, +1.0f), VertexColour(1.0f, 0.0f, 0.0f, 1.0f) },
		{ VertexPosition(-1.0f, +1.0f, +1.0f), VertexColour(1.0f, 0.0f, 0.0f, 1.0f) },
		{ VertexPosition(-1.0f, +1.0f, -1.0f), VertexColour(1.0f, 0.0f, 0.0f, 1.0f) },
		{ VertexPosition(-1.0f, -1.0f, -1.0f), VertexColour(1.0f, 0.0f, 0.0f, 1.0f) },
		{ VertexPosition(+1.0f, -1.0f, -1.0f), VertexColour(1.0f, 0.0f, 0.0f, 1.0f) },
		{ VertexPosition(+1.0f, +1.0f, -1.0f), VertexColour(1.0f, 0.0f, 0.0f, 1.0f) },
		{ VertexPosition(+1.0f, +1.0f, +1.0f), VertexColour(1.0f, 0.0f, 0.0f, 1.0f) },
		{ VertexPosition(+1.0f, -1.0f, +1.0f), VertexColour(1.0f, 0.0f, 0.0f, 1.0f) }
	};*/

	// Create vertex buffer
	D3D11_BUFFER_DESC vertexbuffer_desc = {};
	vertexbuffer_desc.Usage = D3D11_USAGE_DEFAULT;
	vertexbuffer_desc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * m_Vertices.size());
	vertexbuffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertex_subdata = {};
	vertex_subdata.pSysMem = m_Vertices.data();

	DX::Check(device->CreateBuffer(&vertexbuffer_desc, &vertex_subdata, m_VertexBuffer.ReleaseAndGetAddressOf()));
}

void Model::CreateIndexBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Set Indices
	m_IndexCount = static_cast<UINT>(m_Indices.size());

	// Create index buffer
	D3D11_BUFFER_DESC index_buffer_desc = {};
	index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	index_buffer_desc.ByteWidth = static_cast<UINT>(sizeof(UINT) * m_Indices.size());
	index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA index_subdata = {};
	index_subdata.pSysMem = m_Indices.data();

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