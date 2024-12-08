#pragma once

#include <d3d11.h>

// This include is requires for using DirectX smart pointers (ComPtr)
#include <wrl\client.h>
using Microsoft::WRL::ComPtr;

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

	void SetTexture(ID3D11ShaderResourceView* texture);
	void SetDefaultTexture();

private:
	// Number of indices to draw
	UINT m_IndexCount = 0;

	// Vertex buffer
	void CreateVertexBuffer();
	ComPtr<ID3D11Buffer> m_VertexBuffer = nullptr;

	// Index buffer
	void CreateIndexBuffer();
	ComPtr<ID3D11Buffer> m_IndexBuffer = nullptr;

	// Texture buffer
	void LoadTexture();
	ComPtr<ID3D11ShaderResourceView> m_DiffuseTexture = nullptr;
	ComPtr<ID3D11ShaderResourceView> m_DefaultDiffuseTexture = nullptr;
};