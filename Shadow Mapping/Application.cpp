#include "Application.h"
#include "Window.h"
#include "Timer.h"
#include "Renderer.h"
#include "DefaultShader.h"
#include "LineShader.h"
#include "FreeCamera.h"
#include "VisualCamera.h"
#include "ShadowCamera.h"
#include "Model.h"
#include "Floor.h"
#include "Vertex.h"
#include "ShadowMap.h"

#include <DirectXMath.h>
#include <DirectXCollision.h>
using namespace DirectX;

#include <windowsx.h>
#include <iostream>
#include <array>
#include <string>

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

	// Create shadow map
	m_ShadowMap = std::make_unique<ShadowMap>(m_Renderer.get());

	// Used for visualiation
	this->CreateLineBuffer();

	// Create shader
	m_DefaultShader = std::make_unique<DefaultShader>(m_Renderer.get());
	m_DefaultShader->Load();

	// Create shader
	m_LineShader = std::make_unique<LineShader>(m_Renderer.get());
	m_LineShader->Load();

	// Create camera
	m_FreeCamera = std::make_unique<FreeCamera>(window_width, window_height);
	m_VisualCamera = std::make_unique<VisualCamera>(window_width, window_height);
	m_ShadowCamera = std::make_unique<ShadowCamera>(window_width, window_height);

	ID3D11SamplerState* shadow_sampler = m_ShadowMap->GetShadowSamplerState();
	m_Renderer->GetDeviceContext()->PSSetSamplers(0, 1, &shadow_sampler);

	// Print some info
	std::cout << "1) Free camera\n2) Visual camera\n3) Shadow camera" << '\n';
}

int Application::Execute()
{
	m_Timer.Start();

	// Model
	m_Floor = std::make_unique<Floor>(m_Renderer.get());
	m_Floor->Create();

	m_Model = std::make_unique<Model>(m_Renderer.get());
	m_Model->Create();

	// Light direction
	XMFLOAT4 light_direction = DirectX::XMFLOAT4(0.7f, -0.6f, 0.4f, 1.0f);

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
			if (m_CameraToggle == CameraToggle::Visual)
			{
				m_VisualCamera->Move(m_Timer.DeltaTime());
			}
			else if (m_CameraToggle == CameraToggle::Free)
			{
				m_FreeCamera->Move(m_Timer.DeltaTime());
			}

			// Calculate light view and projection
			m_ShadowCamera->LookAt(m_FreeCamera.get(), m_FreeCamera->GetPosition(), XMLoadFloat4(&light_direction));

			// Update light buffer
			m_DefaultShader->UpdateDirectionalLightBuffer(light_direction, m_ShadowCamera->GetView(), m_ShadowCamera->GetProjection());

			// Must render the scene to generate the shadow map
			this->RenderShadowsPass();

			// Render the scene again this time applying the shadow map
			this->RenderMainPass();

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
					if (m_CameraToggle != CameraToggle::Free)
					{
						std::cout << "Free camera\n";
						m_CameraToggle = CameraToggle::Free;
					}
					break;
				case '2':
					if (m_CameraToggle != CameraToggle::Visual)
					{
						std::cout << "Visual camera\n";
						m_CameraToggle = CameraToggle::Visual;

						m_VisualCamera->SetPosition(m_FreeCamera->GetPosition());
					}
					break;
				case '3':
					if (m_CameraToggle != CameraToggle::Shadow)
					{
						std::cout << "Shadow camera\n";
						m_CameraToggle = CameraToggle::Shadow;
					}
					break;
			}

			return 0;
		}
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void Application::RenderShadowsPass()
{
	// Unbind shadow map from the pipeline so we can render the depth
	ID3D11ShaderResourceView* nullSRV = nullptr;
	m_Renderer->GetDeviceContext()->PSSetShaderResources(0, 1, &nullSRV);

	// Bind the shadow map render target
	m_ShadowMap->Bind();

	// Bind the shader to the pipeline
	m_DefaultShader->Use(false);

	// Set camera constant buffer from the shadow camera
	XMMATRIX view = m_ShadowCamera->GetView();
	XMMATRIX projection = m_ShadowCamera->GetProjection();
	XMFLOAT3 position = m_ShadowCamera->GetPosition();
	m_DefaultShader->UpdateCameraBuffer(view, projection, position);

	// Render the scene
	this->RenderScene();
}

void Application::RenderMainPass()
{
	// Set viewport back to scene
	int width, height;
	m_Window->GetSize(&width, &height);
	m_Renderer->SetViewport(width, height);

	// Bind default raster
	m_Renderer->SetRasterState();

	// Clear the buffers
	m_Renderer->Clear();

	// Bind the shader to the pipeline
	m_DefaultShader->Use(true);

	// Set camera constant buffer
	this->UpdateCameraConstantBuffer();

	// Bind shadow map to the pipeline
	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();
	ID3D11ShaderResourceView* shadowmap_texture = m_ShadowMap->GetShadowMapTexture();
	context->PSSetShaderResources(0, 1, &shadowmap_texture);

	ID3D11SamplerState* shadow_sampler = m_ShadowMap->GetShadowSamplerState();
	context->PSSetSamplers(0, 1, &shadow_sampler);

	// Render the scene
	this->RenderScene();
}

void Application::RenderScene()
{
	// Render the floor
	XMMATRIX floor_transform = XMMatrixIdentity();
	floor_transform *= XMMatrixTranslation(0.0f, -1.0f, 0.0f);
	this->UpdateModelConstantBuffer(floor_transform);
	m_Floor->Render();

	// Render the model as giant
	XMMATRIX model_transform = XMMatrixIdentity();
	model_transform *= XMMatrixScaling(10.0f, 10.0f, 10.0f);
	model_transform *= XMMatrixTranslation(0.0f, 5.0f, -50.0f);
	this->UpdateModelConstantBuffer(model_transform);
	m_Model->Render();

	// Render small models
	for (int x = -50; x <= 50; x += 8)
	{
		for (int w = -50; w <= 50; w += 8)
		{
			model_transform = XMMatrixIdentity();
			model_transform *= XMMatrixTranslation(x, 0.0f, w);
			this->UpdateModelConstantBuffer(model_transform);
			m_Model->Render();
		}
	}

	// Visualize orbitial camera frustum
	if (m_CameraToggle == CameraToggle::Visual)
	{
		m_LineShader->Use();
		this->VisualizeCameraFrustum();
		this->VisualizeShadowCamera();
		this->VisualizeLightDirection();
	}
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
	m_FreeCamera->UpdateAspectRatio(window_width, window_height);
	m_VisualCamera->UpdateAspectRatio(window_width, window_height);
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

		if (m_CameraToggle == CameraToggle::Visual)
		{
			m_VisualCamera->Rotate(pitch, yaw);
		}
		else if (m_CameraToggle == CameraToggle::Free)
		{
			m_FreeCamera->Rotate(pitch, yaw);
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
	m_DefaultShader->UpdateModelBuffer(world);
	m_LineShader->UpdateModelBuffer(XMMatrixIdentity());
}

void Application::UpdateCameraConstantBuffer()
{
	if (m_CameraToggle == CameraToggle::Visual)
	{
		XMMATRIX view = m_VisualCamera->GetView();
		XMMATRIX projection = m_VisualCamera->GetProjection();
		XMFLOAT3 position = m_VisualCamera->GetPosition();

		m_DefaultShader->UpdateCameraBuffer(view, projection, position);
		m_LineShader->UpdateCameraBuffer(view, projection, position);
	}
	else if (m_CameraToggle == CameraToggle::Free)
	{
		XMMATRIX view = m_FreeCamera->GetView();
		XMMATRIX projection = m_FreeCamera->GetProjection();
		XMFLOAT3 position = m_FreeCamera->GetPosition();

		m_DefaultShader->UpdateCameraBuffer(view, projection, position);
		m_LineShader->UpdateCameraBuffer(view, projection, position);
	}
	else if (m_CameraToggle == CameraToggle::Shadow)
	{
		XMMATRIX view = m_ShadowCamera->GetView();
		XMMATRIX projection = m_ShadowCamera->GetProjection();
		XMFLOAT3 position = m_ShadowCamera->GetPosition();

		m_DefaultShader->UpdateCameraBuffer(view, projection, position);
		m_LineShader->UpdateCameraBuffer(view, projection, position);
	}
}

void Application::VisualizeCameraFrustum()
{
	// Get bounding frustum from camera
	BoundingFrustum bounding_frustum;
	BoundingFrustum::CreateFromMatrix(bounding_frustum, m_FreeCamera->GetProjection());

	XMMATRIX view_inverse = XMMatrixInverse(nullptr, m_FreeCamera->GetView());
	bounding_frustum.Transform(bounding_frustum, view_inverse);

	// Calculate the edges to render the lines
	std::array<XMFLOAT3, 8> corners;
	bounding_frustum.GetCorners(corners.data());

	// Build line list (24 vertices)
	std::array<LineVertex, 24> line_vertices;

	static const uint32_t edges[12][2] =
	{
		{0,1}, {1,2}, {2,3}, {3,0}, // Near
		{4,5}, {5,6}, {6,7}, {7,4}, // Far
		{0,4}, {1,5}, {2,6}, {3,7}  // Connections
	};

	int v = 0;
	for (auto& edge : edges)
	{
		line_vertices[v].position.x = corners[edge[0]].x;
		line_vertices[v].position.y = corners[edge[0]].y;
		line_vertices[v].position.z = corners[edge[0]].z;
		line_vertices[v].colour = VertexColour(1.0f, 0.0f, 0.0f);
		v++;

		line_vertices[v].position.x = corners[edge[1]].x;
		line_vertices[v].position.y = corners[edge[1]].y;
		line_vertices[v].position.z = corners[edge[1]].z;
		line_vertices[v].colour = VertexColour(1.0f, 0.0f, 0.0f);
		v++;
	}

	// Map lines to the buffer
	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();

	D3D11_MAPPED_SUBRESOURCE resource = {};
	DX::Check(context->Map(m_LineBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource));
	std::memcpy(resource.pData, line_vertices.data(), line_vertices.size() * sizeof(Vertex));
	context->Unmap(m_LineBuffer.Get(), 0);

	// Render
	this->RenderDebugLines();
}

void Application::VisualizeShadowCamera()
{
	XMMATRIX view = m_ShadowCamera->GetView();
	XMMATRIX projection = m_ShadowCamera->GetProjection();

	// NDC 
	XMVECTOR ndcCorners[8] =
	{
		{-1,  1, 0, 1}, { 1,  1, 0, 1}, {-1, -1, 0, 1}, { 1, -1, 0, 1}, // Near
		{-1,  1, 1, 1}, { 1,  1, 1, 1}, {-1, -1, 1, 1}, { 1, -1, 1, 1}  // Far
	};

	// Calculate the inverse of the view projection
	XMMATRIX view_projection = view * projection;
	XMMATRIX inverse_view_projection = XMMatrixInverse(nullptr, view_projection);

	// Get corners
	XMVECTOR corners[8];
	for (int i = 0; i < 8; ++i)
	{
		corners[i] = XMVector3TransformCoord(ndcCorners[i], inverse_view_projection);
	}

	// Build line list (24 vertices)
	std::array<LineVertex, 24> line_vertices;

	static const uint32_t edges[12][2] =
	{
		// Near face (clockwise)
		{0,1}, {1,3}, {3,2}, {2,0},

		// Far face (clockwise)
		{4,5}, {5,7}, {7,6}, {6,4},

		// Near -> Far connections
		{0,4}, {1,5}, {2,6}, {3,7}
	};


	int v = 0;
	for (auto& edge : edges)
	{
		XMFLOAT3 corner0;
		XMFLOAT3 corner1;

		XMStoreFloat3(&corner0, corners[edge[0]]);
		XMStoreFloat3(&corner1, corners[edge[1]]);

		line_vertices[v].position.x = corner0.x;
		line_vertices[v].position.y = corner0.y;
		line_vertices[v].position.z = corner0.z;
		line_vertices[v].colour = VertexColour(0.0f, 1.0f, 0.0f);
		v++;

		line_vertices[v].position.x = corner1.x;
		line_vertices[v].position.y = corner1.y;
		line_vertices[v].position.z = corner1.z;
		line_vertices[v].colour = VertexColour(0.0f, 1.0f, 0.0f);
		v++;
	}

	// Map lines to the buffer
	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();

	D3D11_MAPPED_SUBRESOURCE resource = {};
	DX::Check(context->Map(m_LineBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource));
	std::memcpy(resource.pData, line_vertices.data(), line_vertices.size() * sizeof(Vertex));
	context->Unmap(m_LineBuffer.Get(), 0);

	// Render
	this->RenderDebugLines();
}

void Application::VisualizeLightDirection()
{
	XMFLOAT3 shadow_camera_position = m_ShadowCamera->GetPosition();

	XMVECTOR camera_vec = XMLoadFloat3(&shadow_camera_position);
	XMVECTOR light_direction = (m_ShadowCamera->GetLightDirection() * 50.0f);

	XMFLOAT3 light_position;
	XMStoreFloat3(&light_position, light_direction);

	// Build light line
	std::array<LineVertex, 2> line_vertices;
	line_vertices[0].position = VertexPosition(shadow_camera_position.x, shadow_camera_position.y, shadow_camera_position.z);
	line_vertices[0].colour = VertexColour(1.0f, 1.0f, 0.0f);

	line_vertices[1].position = VertexPosition(light_position.x, light_position.y, light_position.z);
	line_vertices[1].colour = VertexColour(1.0f, 1.0f, 0.0f);

	// Map lines to the buffer
	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();

	D3D11_MAPPED_SUBRESOURCE resource = {};
	DX::Check(context->Map(m_LineBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource));
	std::memcpy(resource.pData, line_vertices.data(), line_vertices.size() * sizeof(Vertex));
	context->Unmap(m_LineBuffer.Get(), 0);

	// Render
	UINT stride = sizeof(LineVertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, m_LineBuffer.GetAddressOf(), &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	context->Draw(2, 0);
}

void Application::CreateLineBuffer()
{
	ID3D11Device* device = m_Renderer->GetDevice();

	// Create vertex buffer
	D3D11_BUFFER_DESC vertexbuffer_desc = {};
	vertexbuffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	vertexbuffer_desc.ByteWidth = static_cast<UINT>(sizeof(LineVertex) * 24);
	vertexbuffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexbuffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	DX::Check(device->CreateBuffer(&vertexbuffer_desc, nullptr, m_LineBuffer.ReleaseAndGetAddressOf()));
}

void Application::RenderDebugLines()
{
	ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();

	UINT stride = sizeof(LineVertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, m_LineBuffer.GetAddressOf(), &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	context->Draw(24, 0);
}
