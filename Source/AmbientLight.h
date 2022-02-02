#pragma once
#include "Globals.h"
#include "imgui.h"

class Archive;

class AmbientLight
{
public:
	AmbientLight();
	~AmbientLight() = default;

	void Update(unsigned int program);

	bool active = true;
	bool selected = false;

	void printGameObjectInfo();
	void Save(Archive* archive);

	float3 color;
	float strength;
};

