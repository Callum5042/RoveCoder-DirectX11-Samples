#pragma once

#include <d3d11.h>
#include <string>

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

	// Load all textures
	void LoadTextures(const std::wstring& path1, const std::wstring& path2, const std::wstring& path3);

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
	ComPtr<ID3D11ShaderResourceView> m_DiffuseTexture1 = nullptr;
	ComPtr<ID3D11ShaderResourceView> m_DiffuseTexture2 = nullptr;
	ComPtr<ID3D11ShaderResourceView> m_DiffuseTexture3 = nullptr;
};