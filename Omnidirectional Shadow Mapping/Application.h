#pragma once

#include "Timer.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <memory>
#include <string>
#include <vector>

#include <DirectXMath.h>
using namespace DirectX;

#include <d3d11_1.h>

// This include is requires for using DirectX smart pointers (ComPtr)
#include <wrl\client.h>
using Microsoft::WRL::ComPtr;

class Window;
class Renderer;

class DefaultShader;
class LineShader;

class FreeCamera;
class VisualCamera;
class ShadowCamera;

class Model;
class Floor;

enum class CameraToggle
{
	Visual,
	Free,
	Shadow,
	ShadowPlusX,
	ShadowMinusX,
	ShadowPlusY,
	ShadowMinusY,
	ShadowPlusZ,
	ShadowMinusZ,
};

class Application
{
public:
	Application();
	virtual ~Application() = default;

	int Execute();

	LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Get window
	inline Window* GetWindow() const { return m_Window.get(); }

private:
	Timer m_Timer;
	std::unique_ptr<Window> m_Window = nullptr;
	std::unique_ptr<Renderer> m_Renderer = nullptr;

	void RenderPointShadowPass();
	void RenderMainPass();

	void RenderScene();

	// Models
	std::unique_ptr<Model> m_Model = nullptr;
	std::unique_ptr<Floor> m_Floor = nullptr;

	// Shaders
	std::unique_ptr<DefaultShader> m_DefaultShader = nullptr;
	std::unique_ptr<LineShader> m_LineShader = nullptr;

	// Cameras
	CameraToggle m_CameraToggle = CameraToggle::Free;
	std::unique_ptr<FreeCamera> m_FreeCamera = nullptr;
	std::unique_ptr<VisualCamera> m_VisualCamera = nullptr;
	std::unique_ptr<ShadowCamera> m_ShadowCamera = nullptr;

	// Internals
	bool m_Running = true;
	bool m_WindowCreated = false;
	std::string m_ApplicationTitle = "Omnidirectional Shadow Mapping";

	// On resized event
	void OnResized(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// On mouse move event
	void OnMouseMove(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Calculate frame stats
	void CalculateFrameStats();
	int m_FrameCount = 0;

	// Compute model view projection of the camera
	void UpdateModelConstantBuffer(const DirectX::XMMATRIX& world);

	// Update camera constant buffer shader
	void UpdateCameraConstantBuffer();

	// Visualize bounding frustum
	void VisualizeCameraFrustum();
	void VisualizeShadowCamera();
	void VisualizeLightDirection();

	// Render debug lines
	ComPtr<ID3D11Buffer> m_LineBuffer;
	void CreateLineBuffer();
	void RenderDebugLines();

	// Point light data
	DirectX::XMFLOAT3 m_PointLight;
	ComPtr<ID3D11Texture2D> m_CubeTex = nullptr;
	ComPtr<ID3D11ShaderResourceView> m_ShadowCubeMap = nullptr;
	std::vector<ComPtr<ID3D11DepthStencilView>> m_TextureDepthStencilViews;
	void CreateRenderToTextureDepthStencilView();
};