#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

// This include is requires for using DirectX smart pointers (ComPtr)
#include <wrl\client.h>
using Microsoft::WRL::ComPtr;

#include <string>

class Renderer;

enum class ShaderType
{
	Vertex,
	Pixel,
};

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
	void UpdateModelViewProjectionBuffer(const DirectX::XMMATRIX& matrix);

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

	// Compile shader from source file
	ComPtr<ID3DBlob> CompileShader(const std::wstring& path, ShaderType shader_type);
};