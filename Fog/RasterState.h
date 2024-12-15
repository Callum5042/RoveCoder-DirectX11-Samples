#pragma once

#include <d3d11.h>

// This include is requires for using DirectX smart pointers (ComPtr)
#include <wrl\client.h>
using Microsoft::WRL::ComPtr;

class Renderer;

class RasterState
{
	Renderer* m_Renderer = nullptr;

public:
	RasterState(Renderer* renderer);
	virtual ~RasterState() = default;

	// Bind the raster state to the pipeline
	void Use();

	// Toggle between solid and wireframe
	void ToggleWireframe();

private:
	bool m_RenderWireframe = false;

	ComPtr<ID3D11RasterizerState> m_RasterStateSolid = nullptr;
	void CreateRasterStateSolid();

	ComPtr<ID3D11RasterizerState> m_RasterStateWireframe = nullptr;
	void CreateRasterStateWireframe();
};