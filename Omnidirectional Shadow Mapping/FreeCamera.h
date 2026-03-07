#pragma once

#include <DirectXMath.h>
using namespace DirectX;

class FreeCamera
{
public:
	FreeCamera(int width, int height);
	virtual ~FreeCamera() = default;

	// Move the camera
	void Move(float delta_time);

	// Recalculates the view based on the pitch and yaw
	void Rotate(float pitch, float yaw);

	// Update aspect ratio
	void UpdateAspectRatio(int width, int height);

	// Set field of view
	void UpdateFov(float fov);

	// Set camera position
	void SetPosition(const XMFLOAT3& position);

	// Set camera pitch and yaw
	void SetPitchAndYaw(float pitch_radians, float yaw_radians);

	// Get projection matrix
	inline const XMMATRIX& GetProjection() const { return m_Projection; }

	// Get view matrix
	inline const XMMATRIX& GetView() const { return m_View; }

	// Get camera position in world space
	inline const XMFLOAT3& GetPosition() const { return m_Position; }

private:
	// Projection matrix
	DirectX::XMMATRIX m_Projection;

	// View matrix
	DirectX::XMMATRIX m_View;

	// Position
	DirectX::XMFLOAT3 m_Position;

	// Camera pitch in radians
	float m_PitchRadians = 0.0f;

	// Camera yaw in radians
	float m_YawRadians = 0.0f;

	// Camera field of view in degrees
	float m_FieldOfViewDegrees = 50.0f;

	// Aspect ratio
	float m_AspectRatio = 0.0f;

	// Recalculates the projection based on the new window size
	void CalculateProjection();

	// Camera speed
	float m_CameraSpeed = 1.0f;
};