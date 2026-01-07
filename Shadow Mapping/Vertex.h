#pragma once

struct VertexPosition
{
	VertexPosition() {}
	VertexPosition(float x, float y, float z) : x(x), y(y), z(z) {}

	float x = 0;
	float y = 0;
	float z = 0;
};

struct VertexNormal
{
	VertexNormal() {}
	VertexNormal(float nx, float ny, float nz) : normal_x(nx), normal_y(ny), normal_z(nz) {}

	float normal_x = 0;
	float normal_y = 0;
	float normal_z = 0;
};

struct VertexColour
{
	VertexColour() {}
	VertexColour(float r, float g, float b) : r(r), g(g), b(b) {}

	float r = 0;
	float g = 0;
	float b = 0;
};

struct Vertex
{
	VertexPosition position;
	VertexNormal normal;
};

struct LineVertex
{
	VertexPosition position;
	VertexColour colour;
};