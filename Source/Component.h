#pragma once
#include "Globals.h"
#include <vector>
#include "Geometry/Triangle.h"
#include "Geometry/AABB.h"

enum class CompType
{
	Transform = 0,
	Mesh,
	Material
};

class Component
{
public:
	Component() = default;
	virtual ~Component() = default;

	unsigned int getId() { return id; };
	CompType getType() { return type; };

	virtual void Update(unsigned int program, float3 &position, Quat &rotation, float3 &scale) {};
	virtual void DebugDraw() {};
	virtual void getBoundingBox(math::AABB &bbox) {} ;
	virtual void getTriangles(std::vector<math::Triangle> &triangles) {} ;
	virtual void printComponentInfo() {};

protected:
	unsigned int id;
	CompType type;
};