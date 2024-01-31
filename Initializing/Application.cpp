#include "Application.h"
#include "Renderer.h"
#include "Window.h"
#include "Timer.h"
#include <iostream>

Application::Application()
{
	// Create window
	m_Window = std::make_unique<Window>(this);
	m_WindowCreated = m_Window->Create("Initializing", 800, 600, false);

	// Create renderer
	m_Renderer = std::make_unique<Renderer>(this);
	m_Renderer->Create();
}

int Application::Execute()
{
	Timer timer;
	timer.Start();

	// Main application loop
	while (m_Running)
	{
		// timer.Tick();
		// this->CalculateFrameStats(timer.DeltaTime());

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

		case WM_SIZE:
			this->OnResized(hwnd, msg, wParam, lParam);
			return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void Application::OnResized(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Window resized is called upon window creation, so ignore if the window has not finished being created
	if (!m_WindowCreated)
		return;

	// Get window size
	int window_width = LOWORD(lParam);
	int window_height = HIWORD(lParam);

	// Resize renderer
	m_Renderer->Resize(window_width, window_height);
}

void Application::CalculateFrameStats(float delta_time)
{
	static float time = 0.0f;

	m_FrameCount++;
	time += delta_time;

	// Update window title every second with FPS
	if (time > 1.0f)
	{
		m_Window->SetTitle("Initializing (FPS: " + std::to_string(m_FrameCount) + ")");

		time = 0.0f;
		m_FrameCount = 0;
	}
}