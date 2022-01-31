#pragma once
#include "Globals.h"
#include <vector>
#include "Geometry/Triangle.h"
#include "Geometry/AABB.h"

enum class CompType
{
	Transform = 0,
	Mesh,
	Material,
	Light,
	Camera
};

class Component
{
public:
	Component() { id = 0; type = CompType::Transform; };
	virtual ~Component() = default;

	unsigned int getId() { return id; };
	CompType getType() { return type; };

	virtual void UpdateTransform(float3& position, Quat& rotation, float3& scale) {};
	virtual void UpdateLight(unsigned int program, const float3& position, const Quat& rotation, const float3& scale) {};
	virtual void Update(unsigned int program, const float3 &position, const Quat &rotation, const float3 &scale) {};
	virtual void DebugDraw() {};
	virtual void getBoundingBox(math::AABB &bbox) {} ;
	virtual void getTriangles(std::vector<math::Triangle> &triangles) {} ;
	virtual void printComponentInfo() {};

	bool render = false;
	bool selected = false;

protected:
	unsigned int id;
	CompType type;
};