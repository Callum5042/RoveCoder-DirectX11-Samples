#pragma once

struct VertexPosition
{
	VertexPosition(float x, float y, float z) : x(x), y(y), z(z) {}

	float x = 0;
	float y = 0;
	float z = 0;
};

struct VertexNormal
{
	VertexNormal(float nx, float ny, float nz) : normal_x(nx), normal_y(ny), normal_z(nz) {}

	float normal_x = 0;
	float normal_y = 0;
	float normal_z = 0;
};

struct Vertex
{
	VertexPosition position;
	VertexNormal normal;
};