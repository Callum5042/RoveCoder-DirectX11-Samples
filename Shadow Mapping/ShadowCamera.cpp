#include "ShadowCamera.h"
#include <algorithm>

#include <DirectXMath.h>
using namespace DirectX;

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

ShadowCamera::ShadowCamera(int width, int height)
{
	m_Position = XMFLOAT3(0.0f, 5.0f, -10.0f);
	CalculateProjection();
}

void ShadowCamera::LookAt(const XMVECTOR& light_direction)
{
	XMVECTOR direction = XMVector3Normalize(light_direction);

	const XMVECTOR global_up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

	const float radius = 10.0f;
	XMVECTOR position = -direction * radius;
	XMStoreFloat3(&m_Position, position);

	XMVECTOR eye = position;
	XMVECTOR to = direction;
	m_View = XMMatrixLookToLH(eye, to, global_up);
}

void ShadowCamera::SetPosition(const XMFLOAT3& position)
{
	m_Position = position;
}

void ShadowCamera::CalculateProjection()
{
	// Calculate ShadowCamera's perspective
	m_Projection = XMMatrixOrthographicLH(20.0f, 20.0f, 1.0f, 20.0f);
}
