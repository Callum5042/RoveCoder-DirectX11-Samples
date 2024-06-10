#pragma once

struct VertexPosition
{
	VertexPosition() {}
	VertexPosition(float x, float y, float z) : x(x), y(y), z(z) {}

	float x = 0;
	float y = 0;
	float z = 0;
};

struct VertexColour
{
	VertexColour() {}
	VertexColour(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

	float r = 0;
	float g = 0;
	float b = 0;
	float a = 0;
};

struct Vertex
{
	VertexPosition position;
	VertexColour colour;
};