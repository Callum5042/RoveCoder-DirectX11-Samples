#pragma once

#include <DirectXMath.h>
using namespace DirectX;

// Perspective free camera
class ShadowCamera
{
public:
	ShadowCamera(int width, int height);
	virtual ~ShadowCamera() = default;

	// Look at direction
	void LookAt(const XMVECTOR& light_direction);

	// Set field of view
	void UpdateFov(float fov);

	// Set camera position
	void SetPosition(const XMFLOAT3& position);

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

	// Recalculates the projection based on the new window size
	void CalculateProjection();

	// Light direction
	XMVECTOR m_LightDirection;
};