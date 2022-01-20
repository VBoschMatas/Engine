#pragma once
#include "Globals.h"
#include "GameObject.h"
#include "ModuleTexture.h"
#include <vector>
#include <map>

class GameObject;

class Scene
{
public:
	void setName(const char* _name) { name = _name; };

	unsigned int getId() { return id; };
	std::string getName() { return name; };

	void AddGameObject(const std::string file_name, GoType type = GoType::Empty);
	void RemoveGameObject(unsigned int id);

private:
	unsigned int id;
	std::string name;
	std::vector<GameObject*> children; // GameObjects that are directly attached to the root
	unsigned int last_go_id;

	// List of all GameObjects, Textures and Materials used in the scene
	std::vector<GameObject> game_objects;
	//std::map<unsigned int, Texture> model_textures;
	//std::vector<Material> model_materials;
};

