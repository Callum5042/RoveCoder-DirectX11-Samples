#include "Application.h"
#include "Window.h"
#include "Timer.h"
#include "Renderer.h"
#include "Shader.h"
#include "OrbitalCamera.h"
#include "FreeCamera.h"
#include "Model.h"
#include "Floor.h"

#include <DirectXMath.h>
using namespace DirectX;

#include <windowsx.h>
#include <iostream>

Application::Application()
{
	const int window_width = 800;
	const int window_height = 600;

	// Create window
	m_Window = std::make_unique<Window>(this);
	m_WindowCreated = m_Window->Create(m_ApplicationTitle, window_width, window_height, false);

	// Create renderer
	m_Renderer = std::make_unique<Renderer>(this);
	m_Renderer->Create();

	// Create shader
	m_Shader = std::make_unique<Shader>(m_Renderer.get());
	m_Shader->Load();

	m_Shader->UpdateDirectionalLightBuffer(DirectX::XMFLOAT4(0.7f, -0.6f, 0.4f, 1.0f));

	// Create camera
	m_OrbitalCamera = std::make_unique<OrbitalCamera>(window_width, window_height);
	m_FreeCamera = std::make_unique<FreeCamera>(window_width, window_height);
}

int Application::Execute()
{
	m_Timer.Start();

	// Model
	m_Floor = std::make_unique<Floor>(m_Renderer.get());
	m_Floor->Create();

	m_Model = std::make_unique<Model>(m_Renderer.get());
	m_Model->Create();

	// Main application loop
	while (m_Running)
	{
		m_Timer.Tick();
		this->CalculateFrameStats();

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
			// Update camera
			if (m_CameraToggle == CameraToggle::Free)
			{
				m_FreeCamera->Move(m_Timer.DeltaTime());
			}

			// Clear the buffers
			m_Renderer->Clear();

			// Bind the shader to the pipeline
			m_Shader->Use();

			// Set camera constant buffer
			this->UpdateCameraConstantBuffer();

			// Render the floor
			DirectX::XMMATRIX floor_transform = DirectX::XMMatrixIdentity();
			floor_transform *= DirectX::XMMatrixTranslation(0.0f, -1.0f, 0.0f);
			this->UpdateModelConstantBuffer(floor_transform);
			m_Floor->Render();

			// Render the model
			DirectX::XMMATRIX model_transform = DirectX::XMMatrixIdentity();
			this->UpdateModelConstantBuffer(model_transform);
			m_Model->Render();

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

		case WM_MOUSEMOVE:
			this->OnMouseMove(hwnd, msg, wParam, lParam);
			return 0;

		case WM_KEYDOWN:
		{
			// Toggle between cameras
			switch (wParam)
			{
				case '1':
					if (m_CameraToggle != CameraToggle::Orbital)
					{
						m_CameraToggle = CameraToggle::Orbital;
						std::cout << "Orbital camera\n";
					}
					break;
				case '2':
					if (m_CameraToggle != CameraToggle::Free)
					{
						m_CameraToggle = CameraToggle::Free;
						m_FreeCamera->SetPosition(m_OrbitalCamera->GetPosition());
						m_FreeCamera->SetPitchAndYaw(m_OrbitalCamera->GetPitch(), m_OrbitalCamera->GetYaw());
						std::cout << "Free camera\n";
					}
					break;
				case '3':
					if (m_CameraToggle != CameraToggle::Shadow)
					{
						m_CameraToggle = CameraToggle::Shadow;
						std::cout << "Shadow camera\n";
					}
					break;
			}

			return 0;
		}
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

	// Update camera
	m_OrbitalCamera->UpdateAspectRatio(window_width, window_height);
	m_FreeCamera->UpdateAspectRatio(window_width, window_height);
}

void Application::OnMouseMove(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static int previous_mouse_x = 0;
	static int previous_mouse_y = 0;

	int mouse_x = static_cast<int>(GET_X_LPARAM(lParam));
	int mouse_y = static_cast<int>(GET_Y_LPARAM(lParam));

	if (wParam & MK_LBUTTON)
	{
		float relative_mouse_x = static_cast<float>(mouse_x - previous_mouse_x);
		float relative_mouse_y = static_cast<float>(mouse_y - previous_mouse_y);

		// Rotate camera
		const float rotate_speed = 20.0f;
		float yaw = relative_mouse_x * rotate_speed * m_Timer.DeltaTime();
		float pitch = relative_mouse_y * rotate_speed * m_Timer.DeltaTime();

		if (m_CameraToggle == CameraToggle::Free)
		{
			m_FreeCamera->Rotate(pitch, yaw);
		}
		else if (m_CameraToggle == CameraToggle::Orbital)
		{
			m_OrbitalCamera->Rotate(pitch, yaw);
		}
	}

	previous_mouse_x = mouse_x;
	previous_mouse_y = mouse_y;
}

void Application::CalculateFrameStats()
{
	static float time = 0.0f;

	m_FrameCount++;
	time += m_Timer.DeltaTime();

	// Update window title every second with FPS
	if (time > 1.0f)
	{
		std::string frame_title = "(FPS: " + std::to_string(m_FrameCount) + ")";
		m_Window->SetTitle(m_ApplicationTitle + " " + frame_title);

		time = 0.0f;
		m_FrameCount = 0;
	}
}

void Application::UpdateModelConstantBuffer(const DirectX::XMMATRIX& world)
{
	m_Shader->UpdateModelBuffer(world);
}

void Application::UpdateCameraConstantBuffer()
{
	if (m_CameraToggle == CameraToggle::Free)
	{
		XMMATRIX view = m_FreeCamera->GetView();
		XMMATRIX projection = m_FreeCamera->GetProjection();
		XMFLOAT3 position = m_FreeCamera->GetPosition();

		m_Shader->UpdateCameraBuffer(view, projection, position);
	}
	else if (m_CameraToggle == CameraToggle::Orbital)
	{
		XMMATRIX view = m_OrbitalCamera->GetView();
		XMMATRIX projection = m_OrbitalCamera->GetProjection();
		XMFLOAT3 position = m_OrbitalCamera->GetPosition();

		m_Shader->UpdateCameraBuffer(view, projection, position);
	}
}
