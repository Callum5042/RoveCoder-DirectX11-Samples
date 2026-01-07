#pragma once

#include <DirectXMath.h>
using namespace DirectX;

// Perspective orbital camera
class OrbitalCamera
{
public:
	OrbitalCamera(int width, int height);
	virtual ~OrbitalCamera() = default;

	// Recalculates the view based on the pitch and yaw
	void Rotate(float pitch, float yaw);

	// Update aspect ratio
	void UpdateAspectRatio(int width, int height);

	// Set field of view
	void UpdateFov(float fov);

	// Get projection matrix
	inline const XMMATRIX& GetProjection() const { return m_Projection; }

	// Get view matrix
	inline const XMMATRIX& GetView() const { return m_View; }

	// Get camera position in world space
	inline const XMFLOAT3& GetPosition() const { return m_Position; }

	// Get pitch
	inline float GetPitch() const { return m_PitchRadians; }

	// Get pitch
	inline float GetYaw() const { return m_YawRadians; }

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
};