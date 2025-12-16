#include "Camera.h"
#include <algorithm>
#include <DirectXMath.h>

Camera::Camera(int width, int height)
{
	constexpr float pitch_radians = DirectX::XMConvertToRadians(30.0f);
	this->Rotate(pitch_radians, 0.0f);
	this->UpdateAspectRatio(width, height);
}

void Camera::Rotate(float pitch_radians, float yaw_radians)
{
	m_PitchRadians += pitch_radians;
	m_YawRadians += yaw_radians;
	m_PitchRadians = std::clamp<float>(m_PitchRadians, -(DirectX::XM_PIDIV2 - 0.1f), DirectX::XM_PIDIV2 - 0.1f);

	// Convert Spherical to Cartesian coordinates.
	const float radius = -8.0f;
	DirectX::XMMATRIX rotation_matrix = DirectX::XMMatrixRotationRollPitchYaw(m_PitchRadians, m_YawRadians, 0);
	DirectX::XMVECTOR position = DirectX::XMVectorSet(0.0f, 0.0f, radius, 0.0f);
	position = XMVector3TransformCoord(position, rotation_matrix);

	// Store camera position
	DirectX::XMStoreFloat3(&m_Position, position);

	// Calculate camera's view
	DirectX::XMVECTOR eye = position;
	DirectX::XMVECTOR at = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_View = DirectX::XMMatrixLookAtLH(eye, at, up);
}

void Camera::UpdateAspectRatio(int width, int height)
{
	// Calculate window aspect ratio
	m_AspectRatio = static_cast<float>(width) / height;
	CalculateProjection();
}

void Camera::UpdateFov(float fov)
{
	m_FieldOfViewDegrees += fov;
	m_FieldOfViewDegrees = std::clamp(m_FieldOfViewDegrees, 0.1f, 179.9f);
	CalculateProjection();
}

void Camera::CalculateProjection()
{
	// Convert degrees to radians
	float field_of_view_radians = DirectX::XMConvertToRadians(m_FieldOfViewDegrees);

	// Calculate camera's perspective
	m_Projection = DirectX::XMMatrixPerspectiveFovLH(field_of_view_radians, m_AspectRatio, 0.01f, 100.0f);
}