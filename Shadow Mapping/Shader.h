#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

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
	void UpdateModelBuffer(const XMMATRIX& transform);

	// Update camera buffer
	void UpdateCameraBuffer(const XMMATRIX& view, const XMMATRIX& projection, const XMFLOAT3& position);

	// Update the light buffer
	void UpdateDirectionalLightBuffer(const XMFLOAT4& direction);

private:
	// Create vertex shader
	void LoadVertexShader();
	ComPtr<ID3D11VertexShader> m_VertexShader = nullptr;
	ComPtr<ID3D11InputLayout> m_VertexLayout = nullptr;

	// Create pixel shader
	void LoadPixelShader();
	ComPtr<ID3D11PixelShader> m_PixelShader = nullptr;

	// ModelViewProjection constant buffer
	ComPtr<ID3D11Buffer> m_ModelConstantBuffer = nullptr;
	void CreateModelConstantBuffer();

	// Camera constant buffer
	ComPtr<ID3D11Buffer> m_CameraConstantBuffer = nullptr;
	void CreateCameraConstantBuffer();

	// Create the DirectionalLightBuffer
	ComPtr<ID3D11Buffer> m_DirectionalLightBuffer = nullptr;
	void CreateDirectionalLightBuffer();
};