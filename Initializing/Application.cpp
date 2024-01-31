#include "Application.h"
#include "Renderer.h"
#include "Window.h"
#include <iostream>

Application::Application()
{
	m_Window = std::make_unique<Window>(this);
	m_Renderer = std::make_unique<Renderer>(this);
}

int Application::Execute()
{
	std::cout << "Hello, Application\n";

	m_Window->Create("Initializing", 800, 600, false);
	m_Renderer->Create();

	// Main application loop
	while (m_Running)
	{
		MSG msg = {};
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				m_Running = false;
			}

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		else
		{
			// Clear the buffers
			m_Renderer->Clear();

			// Display the rendered scene
			m_Renderer->Present();
		}
	}

	return 0;
}

LRESULT Application::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}