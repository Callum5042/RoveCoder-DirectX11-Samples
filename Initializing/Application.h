#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <memory>

class Window;
class Renderer;

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

	bool m_Running = true;
};