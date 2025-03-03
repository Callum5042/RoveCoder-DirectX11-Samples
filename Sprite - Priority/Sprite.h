#pragma once

#include <d3d11.h>
#include <vector>

// This include is requires for using DirectX smart pointers (ComPtr)
#include <wrl\client.h>
using Microsoft::WRL::ComPtr;

class Renderer;

struct SpriteVertex
{
	// Vertex position
	float x = 0;
	float y = 0;
	float z = 0;

	// Size
	float width = 0;
	float height = 0;
};

class Sprite
{
	Renderer* m_Renderer = nullptr;

public:
	Sprite(Renderer* renderer);
	virtual ~Sprite() = default;

	// Create device
	void Create();

	// Render the model
	void Render();

private:

	// Vertex buffer
	ComPtr<ID3D11Buffer> m_VertexBuffer = nullptr;
	void CreateVertexBuffer();

	// Vertices
	std::vector<SpriteVertex> m_Vertices;

	// Texture buffer
	void LoadTexture();
	ComPtr<ID3D11ShaderResourceView> m_DiffuseTexture = nullptr;
};