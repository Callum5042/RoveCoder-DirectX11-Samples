#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <memory>
#include <string>

#include <DirectXMath.h>
using namespace DirectX;

class Window;
class Renderer;
class Shader;

class OrbitalCamera;
class FreeCamera;

class Model;
class Floor;

enum class CameraToggle
{
	Free,
	Orbital,
	Shadow
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
	std::unique_ptr<Window> m_Window = nullptr;
	std::unique_ptr<Renderer> m_Renderer = nullptr;
	std::unique_ptr<Shader> m_Shader = nullptr;

	// Models
	std::unique_ptr<Model> m_Model = nullptr;
	std::unique_ptr<Floor> m_Floor = nullptr;

	// Cameras
	CameraToggle m_CameraToggle = CameraToggle::Orbital;
	std::unique_ptr<OrbitalCamera> m_OrbitalCamera = nullptr;
	std::unique_ptr<FreeCamera> m_FreeCamera = nullptr;

	// Internals
	bool m_Running = true;
	bool m_WindowCreated = false;
	std::string m_ApplicationTitle = "Shadow Mapping";

	// On resized event
	void OnResized(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// On mouse move event
	void OnMouseMove(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Calculate frame stats
	void CalculateFrameStats(float delta_time);
	int m_FrameCount = 0;

	// Compute model view projection of the camera
	void UpdateModelConstantBuffer(const DirectX::XMMATRIX& world);

	// Update camera constant buffer shader
	void UpdateCameraConstantBuffer();
};