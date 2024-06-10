#pragma once

#include <d3d11_1.h>
#include <exception>

// This include is requires for using DirectX smart pointers (ComPtr)
#include <wrl\client.h>
using Microsoft::WRL::ComPtr;

class Application;

namespace DX
{
	// Throw if the function result has failed. For ease of error handling
	inline void Check(HRESULT hr)
	{
#ifdef _DEBUG
		if (FAILED(hr))
		{
			throw std::exception();
		}
#endif
	}
}

class Renderer
{
	Application* m_Application = nullptr;

public:
	Renderer(Application* application);
	virtual ~Renderer() = default;

	// Creates the rendering device and context
	void Create();

	// Clear the buffers
	void Clear();

	// Display the rendered scene
	void Present();

	// Resizing
	void Resize(int width, int height);

	// Get render device
	inline ID3D11Device* GetDevice() const { return m_Device.Get(); }

	// Get render context
	inline ID3D11DeviceContext* GetDeviceContext() const { return m_DeviceContext.Get(); }

private:
	// Device and device context
	ComPtr<ID3D11Device> m_Device = nullptr;
	ComPtr<ID3D11DeviceContext> m_DeviceContext = nullptr;
	void CreateDeviceAndContext();

	// Swapchain
	ComPtr<IDXGISwapChain> m_SwapChain = nullptr;
	ComPtr<IDXGISwapChain1> m_SwapChain1 = nullptr;
	void CreateSwapChain(int width, int height);

	// Render target and depth stencil view
	ComPtr<ID3D11RenderTargetView> m_RenderTargetView = nullptr;
	ComPtr<ID3D11DepthStencilView> m_DepthStencilView = nullptr;
	void CreateRenderTargetAndDepthStencilView(int width, int height);

	// Viewport
	void SetViewport(int width, int height);
};