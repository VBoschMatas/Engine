#pragma once
#include "Globals.h"
#include "imgui.h"

class AmbientLight
{
public:
	AmbientLight();
	~AmbientLight() = default;

	void Update(unsigned int program);

	bool active = true;
	bool selected = false;

	void printGameObjectInfo();
private:
	float3 color;
	float strength;
};

