#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <string>

class Application;

std::wstring ConvertToWString(const std::string& str);
std::string ConvertToString(const std::wstring& str);

class Window
{
	Application* m_Application = nullptr;

public:
	Window(Application* application);
	virtual ~Window();

	// Create the window
	void Create(const std::string& title, int width, int height, bool fullscreen);

	// Destroys the window
	void Destroy();

	// Get window size
	void GetSize(int* width, int* height);

	// Get native window handle
	inline HWND GetHwnd() const { return m_Hwnd; }

private:
	HWND m_Hwnd = NULL;
};