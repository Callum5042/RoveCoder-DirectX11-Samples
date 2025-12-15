#pragma once

#include "Vertex.h"

#include <d3d11.h>

#include <DirectXMath.h>
using namespace DirectX;

// This include is requires for using DirectX smart pointers (ComPtr)
#include <wrl\client.h>
using Microsoft::WRL::ComPtr;

#include <vector>

class Renderer;

class Model
{
	Renderer* m_Renderer = nullptr;

public:
	Model(Renderer* renderer);
	virtual ~Model() = default;

	// Create the model
	void Create();

	// Render the model
	void Render();

private:

	// Geometry
	std::vector<VertexPosition> m_VertexPosition;
	std::vector<XMFLOAT3> m_VertexNormal;
	std::vector<VertexTextureUV> m_VertexUV;

	std::vector<UINT> m_Indices;

	// Vertex buffer
	void CreateVertexPositionBuffer();
	void CreateVertexUVBuffer();
	ComPtr<ID3D11Buffer> m_VertexPositionBuffer = nullptr;
	ComPtr<ID3D11Buffer> m_VertexUVBuffer = nullptr;

	// Index buffer
	void CreateIndexBuffer();
	ComPtr<ID3D11Buffer> m_IndexBuffer = nullptr;

	// Texture buffer
	void LoadTexture();
	ComPtr<ID3D11ShaderResourceView> m_DiffuseTexture1 = nullptr;
	ComPtr<ID3D11ShaderResourceView> m_DiffuseTexture2 = nullptr;
};