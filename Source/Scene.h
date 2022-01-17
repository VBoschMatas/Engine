#pragma once
#include "Globals.h"
#include <vector>

class GameObject;

class Scene
{

private:
	std::string name;
	std::vector<GameObject> game_objects;
};

