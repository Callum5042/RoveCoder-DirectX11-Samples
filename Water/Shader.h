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

struct TextureBuffer
{
	DirectX::XMMATRIX matrix1;
	DirectX::XMMATRIX matrix2;
};

__declspec(align(16))
struct GerstnerWave
{
	float Amplitude; // A (Height of the wave)
	float Wavelength; // Lambda (Used to calculate k)
	float Speed; // S (Used to calculate omega)
	float unused1;

	DirectX::XMFLOAT2 Direction; // D (Normalized 2D vector [Dx, Dz])
	float Steepness; // Q (Controls the sharpness of the crests)
	float unused2;
};

__declspec(align(16))
struct GerstnerWaveBuffer
{
	GerstnerWave waves[4];
	float time;

	float padding[3];
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
	void UpdateModelViewProjectionBuffer(const DirectX::XMMATRIX& matrix, float delta_time);

	void UpdateTextureBuffer(const DirectX::XMMATRIX& matrix1, const DirectX::XMMATRIX& matrix2);

	void UpdateWaveBuffer(GerstnerWaveBuffer buffer);

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

	// Texture buffer
	ComPtr<ID3D11Buffer> m_TextureConstantBuffer = nullptr;
	void CreateTextureConstantBuffer();

	// Wave buffer
	ComPtr<ID3D11Buffer> m_WaveConstantBuffer = nullptr;
	void CreateWaveConstantBuffer();

	// Compile shader from source file
	ComPtr<ID3DBlob> CompileShader(const std::wstring& path, ShaderType shader_type);
};