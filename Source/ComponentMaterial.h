#pragma once
#include "Component.h"
#include <vector>

struct Material
{
	unsigned int id;
	std::vector<unsigned int> texture_id;
};

class ComponentMaterial : public Component
{
};

