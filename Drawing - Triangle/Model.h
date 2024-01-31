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

private:
	// Number of vertices to draw
	UINT m_VertexCount = 0;

	// Vertex buffer
	ComPtr<ID3D11Buffer> m_VertexBuffer = nullptr;
};