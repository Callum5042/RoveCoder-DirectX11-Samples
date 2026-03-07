#include "ShadowCamera.h"
#include "FreeCamera.h"
#include "ShadowMap.h"

#include <algorithm>
#include <array>
#include <vector>
#include <cmath>

#include <DirectXMath.h>
#include <DirectXCollision.h>
using namespace DirectX;

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#undef min
#undef max

ShadowCamera::ShadowCamera(int width, int height)
{
	m_Position = XMFLOAT3(0.0f, 5.0f, -10.0f);
}

void ShadowCamera::LookAt(const XMVECTOR& light_direction)
{
	XMVECTOR direction = XMVector3Normalize(light_direction);

	// Calculate view
	const float radius = 10.0f;
	XMVECTOR position = -direction * radius;
	XMStoreFloat3(&m_Position, position);

	XMVECTOR eye = position;
	XMVECTOR to = direction;
	XMVECTOR global_up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	m_View = XMMatrixLookToLH(eye, to, global_up);

	// Calculate projection
	m_Projection = XMMatrixOrthographicLH(20.0f, 20.0f, 1.0f, 20.0f);
}

void ShadowCamera::LookAt(FreeCamera* free_camera, const XMFLOAT3& position, const XMVECTOR& light_direction)
{
	// Tightly fits the light view and projection to the camera frustum

	// Projection
	XMMATRIX view = free_camera->GetView();
	XMMATRIX projection = free_camera->GetProjection();

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
	XMVECTOR frustumCornersWS[8];
	for (int i = 0; i < 8; ++i)
	{
		frustumCornersWS[i] = XMVector3TransformCoord(ndcCorners[i], inverse_view_projection);
	}

	// Calculate camera position
	XMVECTOR center = XMVectorZero();
	for (int i = 0; i < 8; ++i)
	{
		center += frustumCornersWS[i];
	}

	center /= 8;
	XMStoreFloat3(&m_Position, center);

	const XMVECTOR global_up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	XMMATRIX light_view = XMMatrixLookToLH(center, light_direction, global_up);

	// Transform into light space
	XMVECTOR frustumCornersLS[8];
	for (int i = 0; i < 8; ++i)
	{
		frustumCornersLS[i] = XMVector3TransformCoord(frustumCornersWS[i], light_view);
	}

	// Compute min/max extents in light space
	XMVECTOR minExtents = frustumCornersLS[0];
	XMVECTOR maxExtents = frustumCornersLS[0];

	for (int i = 1; i < 8; ++i)
	{
		minExtents = XMVectorMin(minExtents, frustumCornersLS[i]);
		maxExtents = XMVectorMax(maxExtents, frustumCornersLS[i]);
	}

	float minX = XMVectorGetX(minExtents);
	float minY = XMVectorGetY(minExtents);
	float minZ = XMVectorGetZ(minExtents);

	float maxX = XMVectorGetX(maxExtents);
	float maxY = XMVectorGetY(maxExtents);
	float maxZ = XMVectorGetZ(maxExtents);

	// Calculate projection
	XMMATRIX light_projection = XMMatrixOrthographicOffCenterLH(minX, maxX, minY, maxY, minZ, maxZ);

	// Store variables
	m_View = light_view;
	m_Projection = light_projection;
}

void ShadowCamera::SetPosition(const XMFLOAT3& position)
{
	m_Position = position;
}
