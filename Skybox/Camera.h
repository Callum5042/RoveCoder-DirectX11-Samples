#pragma once

#include <DirectXMath.h>

// Perspective orbital camera
class Camera
{
public:
	Camera(int width, int height);
	virtual ~Camera() = default;

	// Recalculates the view based on the pitch and yaw
	void Rotate(float pitch, float yaw);

	// Update aspect ratio
	void UpdateAspectRatio(int width, int height);

	// Set field of view
	void UpdateFov(float fov);

	// Get projection matrix
	inline const DirectX::XMMATRIX& GetProjection() const { return m_Projection; }

	// Get view matrix
	inline const DirectX::XMMATRIX& GetView() const { return m_View; }

	// Get position
	inline const DirectX::XMFLOAT3& GetPosition() const { return m_Position; }

private:
	// Projection matrix
	DirectX::XMMATRIX m_Projection;

	// View matrix
	DirectX::XMMATRIX m_View;

	// Camera position
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
};