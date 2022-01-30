#pragma once
#include "Component.h"

enum class LightType
{
	Directional,
	Point,
	Spot
};

class ComponentLight : public Component
{

public:
	ComponentLight();
	~ComponentLight() = default;

	unsigned int getId() { return id; };

	float3 color;
	LightType Ltype;
private:
	unsigned int id;
};

