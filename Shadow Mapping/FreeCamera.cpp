#include "FreeCamera.h"
#include <algorithm>

#include <DirectXMath.h>
using namespace DirectX;

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

FreeCamera::FreeCamera(int width, int height)
{
	constexpr float pitch_radians = XMConvertToRadians(30.0f);
	this->UpdateAspectRatio(width, height);

	m_CameraSpeed = 5.0f;
	m_Position = XMFLOAT3(0.0f, 5.0f, -10.0f);
}

void FreeCamera::Rotate(float pitch_radians, float yaw_radians)
{
	m_PitchRadians += pitch_radians;
	m_YawRadians += yaw_radians;
	m_PitchRadians = std::clamp<float>(m_PitchRadians, -(DirectX::XM_PIDIV2 - 0.1f), DirectX::XM_PIDIV2 - 0.1f);

	const XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	const XMVECTOR global_forward = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);

	// Calculate the forward direction of the camera
	XMMATRIX rotation_matrix = XMMatrixRotationRollPitchYaw(m_PitchRadians, m_YawRadians, 0);
	XMVECTOR forward = XMVector3Normalize(XMVector3Transform(global_forward, rotation_matrix));
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(up, forward));

	// Recalculate view matrix 
	XMVECTOR eye = XMLoadFloat3(&m_Position);
	XMVECTOR at = forward;
	m_View = XMMatrixLookToLH(eye, at, up);
}

void FreeCamera::Move(float delta_time)
{
	const XMVECTOR global_up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	const XMVECTOR global_forward = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);

	// Calculate the forward direction of the camera
	XMMATRIX rotation_matrix = XMMatrixRotationRollPitchYaw(m_PitchRadians, m_YawRadians, 0);
	XMVECTOR forward = XMVector3Normalize(XMVector3Transform(global_forward, rotation_matrix));
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(global_up, forward));
	XMVECTOR up = XMVector3Normalize(XMVector3Cross(forward, right));

	// Move the camera
	XMVECTOR position = XMLoadFloat3(&m_Position);
	if (GetAsyncKeyState('W') & 0x8000)
	{
		position += forward * m_CameraSpeed * delta_time;
	}
	else if (GetAsyncKeyState('S') & 0x8000)
	{
		position += -forward * m_CameraSpeed * delta_time;
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		position += right * m_CameraSpeed * delta_time;
	}
	else if (GetAsyncKeyState('A') & 0x8000)
	{
		position += -right * m_CameraSpeed * delta_time;
	}

	if (GetAsyncKeyState('E') & 0x8000)
	{
		position += up * m_CameraSpeed * delta_time;
	}
	else if (GetAsyncKeyState('Q') & 0x8000)
	{
		position += -up * m_CameraSpeed * delta_time;
	}

	// Store new camera position
	XMStoreFloat3(&m_Position, position);

	// Recalculate view matrix 
	XMVECTOR eye = position;
	XMVECTOR at = forward;
	m_View = XMMatrixLookToLH(eye, at, global_up);
}

void FreeCamera::UpdateAspectRatio(int width, int height)
{
	// Calculate window aspect ratio
	m_AspectRatio = static_cast<float>(width) / height;
	CalculateProjection();
}

void FreeCamera::UpdateFov(float fov)
{
	m_FieldOfViewDegrees += fov;
	m_FieldOfViewDegrees = std::clamp(m_FieldOfViewDegrees, 0.1f, 179.9f);
	CalculateProjection();
}

void FreeCamera::CalculateProjection()
{
	// Convert degrees to radians
	float field_of_view_radians = XMConvertToRadians(m_FieldOfViewDegrees);

	// Calculate FreeCamera's perspective
	m_Projection = XMMatrixPerspectiveFovLH(field_of_view_radians, m_AspectRatio, 0.01f, 100.0f);
}
