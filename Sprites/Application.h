#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <memory>
#include <string>

class Window;
class Renderer;
class Camera;

class Shader;
class SpriteShader;

class Model;
class Sprite;

class RasterState;
class TextureSampler;

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
	std::unique_ptr<SpriteShader> m_SpriteShader = nullptr;

	std::unique_ptr<Model> m_Model = nullptr;
	std::unique_ptr<Sprite> m_Sprite = nullptr;

	std::unique_ptr<Camera> m_Camera = nullptr;
	std::unique_ptr<RasterState> m_RasterState = nullptr;
	std::unique_ptr<TextureSampler> m_TextureSampler = nullptr;

	bool m_Running = true;
	bool m_WindowCreated = false;
	std::string m_ApplicationTitle = "Sprites";

	// On resized event
	void OnResized(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// On mouse move event
	void OnMouseMove(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// On keydown event
	void OnKeyDown(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Calculate frame stats
	void CalculateFrameStats(float delta_time);
	int m_FrameCount = 0;

	// Compute model view projection of the camera
	void ComputeModelViewProjectionMatrix();
	void UpdateBillboardWorldConstantBuffer();
};