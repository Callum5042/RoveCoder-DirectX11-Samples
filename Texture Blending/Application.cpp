#include "Application.h"
#include "Window.h"
#include "Timer.h"
#include "Renderer.h"
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "RasterState.h"
#include "TextureSampler.h"

#include <DirectXMath.h>
using namespace DirectX;

#include <windowsx.h>

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

	// Create camera
	m_Camera = std::make_unique<Camera>(window_width, window_height);
}

int Application::Execute()
{
	Timer timer;
	timer.Start();

	// Model
	m_ModelFloor = std::make_unique<Model>(m_Renderer.get());
	m_ModelFloor->Create();
	m_ModelFloor->LoadTextures(L"PavingStones142_1K-PNG_Color.png", L"Moss001_1K-PNG_Color.png", L"alpha_map.png");

	m_ModelCube = std::make_unique<Model>(m_Renderer.get());
	m_ModelCube->Create();
	m_ModelCube->LoadTextures(L"water_basecolour.png", L"water_highlights2.png", L"water_highlights.png");

	// Raster state
	m_RasterState = std::make_unique<RasterState>(m_Renderer.get());

	// Texture sampler
	m_TextureSampler = std::make_unique<TextureSampler>(m_Renderer.get());

	// Main application loop
	while (m_Running)
	{
		timer.Tick();
		this->CalculateFrameStats(timer.DeltaTime());

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

			// Bind the shader to the pipeline
			m_Shader->Use();

			// Bind the raster state (solid/wireframe) to the pipeline
			m_RasterState->Use();

			// Bind texture sampler to the pipeline
			m_TextureSampler->Use();

			// Render the floor
			{
				DirectX::XMMATRIX floor_world = DirectX::XMMatrixIdentity();
				floor_world *= DirectX::XMMatrixScaling(5.0f, 0.01f, 5.0f);
				floor_world *= DirectX::XMMatrixTranslation(0.0f, -2.0f, 0.0f);
				this->ComputeModelViewProjectionMatrix(floor_world, TextureBlendMode::Interpolate);
				m_ModelFloor->Render();
			}

			// Render the model
			{
				DirectX::XMMATRIX model_world = DirectX::XMMatrixIdentity();
				model_world *= DirectX::XMMatrixScaling(2.0f, 2.0f, 2.0f);
				model_world *= DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
				this->ComputeModelViewProjectionMatrix(model_world, TextureBlendMode::Screen);
				m_ModelCube->Render();
			}

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
			this->OnKeyDown(hwnd, msg, wParam, lParam);
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

	// Update camera
	m_Camera->UpdateAspectRatio(window_width, window_height);
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
		float yaw = relative_mouse_x * 0.01f;
		float pitch = relative_mouse_y * 0.01f;

		m_Camera->Rotate(pitch, yaw);
	}

	previous_mouse_x = mouse_x;
	previous_mouse_y = mouse_y;
}

void Application::OnKeyDown(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	WORD flags = HIWORD(lParam);
	BOOL key_repeat = (flags & KF_REPEAT) == KF_REPEAT;

	if (!key_repeat)
	{
		m_RasterState->ToggleWireframe();
	}
}

void Application::CalculateFrameStats(float delta_time)
{
	static float time = 0.0f;

	m_FrameCount++;
	time += delta_time;

	// Update window title every second with FPS
	if (time > 1.0f)
	{
		std::string frame_title = "(FPS: " + std::to_string(m_FrameCount) + ")";
		m_Window->SetTitle(m_ApplicationTitle + " " + frame_title);

		time = 0.0f;
		m_FrameCount = 0;
	}
}

void Application::ComputeModelViewProjectionMatrix(const DirectX::XMMATRIX& world, TextureBlendMode mode)
{
	DirectX::XMMATRIX matrix = world;
	matrix *= m_Camera->GetView();
	matrix *= m_Camera->GetProjection();

	m_Shader->UpdateModelViewProjectionBuffer(matrix, static_cast<int>(mode));
}