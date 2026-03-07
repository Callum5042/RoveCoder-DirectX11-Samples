#include "Window.h"
#include "Application.h"

namespace
{
	static Application* GetApplication(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		Application* application = nullptr;
		if (uMsg == WM_NCCREATE)
		{
			CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
			application = reinterpret_cast<Window*>(pCreate->lpCreateParams)->GetApplication();
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(application));
		}
		else
		{
			application = reinterpret_cast<Application*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		}

		return application;
	}

	LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// Application handling
		Application* application = GetApplication(hwnd, msg, wParam, lParam);
		if (application == nullptr)
		{
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}

		return application->HandleMessage(hwnd, msg, wParam, lParam);
	}
}

std::wstring ConvertToWString(const std::string& str)
{
	if (str.empty())
		return std::wstring();

	int str_size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), nullptr, 0);

	std::wstring conversion(str_size, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), conversion.data(), str_size);
	return conversion;
}

std::string ConvertToString(const std::wstring& str)
{
	if (str.empty())
		return std::string();

	int str_size = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);

	std::string conversion;
	conversion.resize(str_size);

	WideCharToMultiByte(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), conversion.data(), str_size, NULL, NULL);
	return conversion;
}

Window::Window(Application* application) : m_Application(application)
{
}

Window::~Window()
{
	this->Destroy();
}

bool Window::Create(const std::string& title, int width, int height, bool fullscreen)
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	std::wstring window_name = ConvertToWString(title);

	// Setup window class
	WNDCLASS wc = {};
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = ::MainWndProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.lpszClassName = window_name.c_str();

	if (!RegisterClass(&wc))
	{
		MessageBox(NULL, L"RegisterClass Failed", L"Error", MB_OK);
		throw std::exception();
	}

	// Create window
	m_Hwnd = CreateWindow(wc.lpszClassName, window_name.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, hInstance, this);
	if (m_Hwnd == NULL)
	{
		MessageBox(NULL, L"CreateWindow Failed", L"Error", MB_OK);
		throw std::exception();
	}

	// Borderless fullscreen
	if (fullscreen)
	{
		SetWindowLong(m_Hwnd, GWL_STYLE, 0);
	}

	// Show window
	int cmd_show = (fullscreen ? SW_MAXIMIZE : SW_SHOWNORMAL);
	ShowWindow(m_Hwnd, cmd_show);

	return true;
}

void Window::Destroy()
{
	DestroyWindow(m_Hwnd);
}

void Window::GetSize(int* width, int* height)
{
	RECT rect;
	GetClientRect(m_Hwnd, &rect);

	*width = rect.right - rect.left;
	*height = rect.bottom - rect.top;
}

void Window::SetTitle(const std::string& title)
{
	SetWindowText(m_Hwnd, ConvertToWString(title).c_str());
}
