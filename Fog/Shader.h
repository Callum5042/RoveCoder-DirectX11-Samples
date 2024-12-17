#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

// This include is requires for using DirectX smart pointers (ComPtr)
#include <wrl\client.h>
using Microsoft::WRL::ComPtr;

class Renderer;

class Shader
{
	Renderer* m_Renderer = nullptr;

public:
	Shader(Renderer* renderer);
	virtual ~Shader() = default;

	// Load the shader
	void Load();

	// Bind shader to the pipeline
	void Use();

	// Update the model view projection constant buffer
	void UpdateModelViewProjectionBuffer(const DirectX::XMMATRIX& matrix, const DirectX::XMMATRIX& world);

	// Update the fog buffer
	void UpdateFogBuffer(const DirectX::XMFLOAT3& camera_eye, float fog_start, float fog_range);

private:
	// Create vertex shader
	void LoadVertexShader();
	ComPtr<ID3D11VertexShader> m_VertexShader = nullptr;
	ComPtr<ID3D11InputLayout> m_VertexLayout = nullptr;

	// Create pixel shader
	void LoadPixelShader();
	ComPtr<ID3D11PixelShader> m_PixelShader = nullptr;

	// ModelViewProjection constant buffer
	ComPtr<ID3D11Buffer> m_ModelViewProjectionConstantBuffer = nullptr;
	void CreateWorldViewProjectionConstantBuffer();

	// Fog constant buffer
	ComPtr<ID3D11Buffer> m_FogConstantBuffer = nullptr;
	void CreateFogConstantBuffer();
};