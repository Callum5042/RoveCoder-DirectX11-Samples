#include "Application.h"
#include "Window.h"
#include "Timer.h"
#include "Renderer.h"
#include "Shader.h"
#include "ColourShader.h"
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

	// Create colour shader
	m_ColourShader = std::make_unique<ColourShader>(m_Renderer.get());
	m_ColourShader->Load();

	// Create camera
	m_Camera = std::make_unique<Camera>(window_width, window_height);
}

int Application::Execute()
{
	Timer timer;
	timer.Start();

	// Model
	m_Model = std::make_unique<Model>(m_Renderer.get());
	m_Model->Create();

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

			// Bind the raster state (solid/wireframe) to the pipeline
			m_RasterState->Use();

			// Bind texture sampler to the pipeline
			m_TextureSampler->Use();

			// Bind the shader to the pipeline
			m_Shader->Use();

			// Write to the stencil buffer in the first pass
			this->WriteToStencilBuffer();

			// Write to the back buffer
			this->WriteToBackBuffer();

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

void Application::ComputeModelViewProjectionMatrix(float scale)
{
	DirectX::XMMATRIX matrix = DirectX::XMMatrixIdentity();
	matrix *= DirectX::XMMatrixScaling(scale, scale, scale);
	matrix *= m_Camera->GetView();
	matrix *= m_Camera->GetProjection();

	m_Shader->UpdateModelViewProjectionBuffer(matrix);
	m_ColourShader->UpdateModelViewProjectionBuffer(matrix);
}

void Application::WriteToStencilBuffer()
{
	// Set the raster to only write to the depth/stencil buffer (ignoring the back buffer)
	m_Renderer->SetStencilRenderTarget();

	// Set to stencil state to write to the stencil buffer
	m_Renderer->SetStencilWriteMask();

	// Update the model view projection constant buffer
	this->ComputeModelViewProjectionMatrix(1.0f);

	// Render the model which will only write to the stencil buffer
	m_Model->Render();
}

void Application::WriteToBackBuffer()
{
	// Allow rendering into the back buffer
	m_Renderer->SetDefaultRenderTarget();

	// Set the stencil state so future rendering will now reject pixels that are masked out
	m_Renderer->SetStencilReadMask();

	// Set a solid colour shader for the outlining
	m_ColourShader->Use();

	// Render the model 2% bigger so the outline will not be rejected by the stencil buffer
	this->ComputeModelViewProjectionMatrix(1.02f);
	m_Model->Render();

	// Turn off the stencil mask by resetting it to the default state
	m_Renderer->ResetStencilMask();

	// Restore back to normal shading
	m_Shader->Use();

	// Render the original object size (stencil state will be different so will no longer reject the pixels)
	this->ComputeModelViewProjectionMatrix(1.0f);
	m_Model->Render();
}