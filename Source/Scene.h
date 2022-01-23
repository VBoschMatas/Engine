#pragma once
#include "Globals.h"
#include "GameObject.h"
#include "Mesh.h"
#include "ModuleTexture.h"
#include <vector>
#include <map>

class GameObject;

struct Material
{
	unsigned int id;
	std::vector<unsigned int> texture_id;
};

class Scene
{
public:
	Scene(unsigned int _id)
	{
		id = _id;
		last_go_id = 0;
	};
	Scene(const char* _name, unsigned int _id) {
		name = _name;
		id = _id;
		last_go_id = 0;
	};
	~Scene() = default;

	void setName(const char* _name) { name = _name; };
	void Update(unsigned int program);

	unsigned int getId() { return id; };
	std::string getName() { return name; };

	void AddGameObject(const std::string file_name, GoType type = GoType::Empty);
	void RemoveGameObject(unsigned int id);

private:
	unsigned int id;
	std::string name;
	std::vector<GameObject*> children; // GameObjects that are directly attached to the root
	unsigned int last_go_id;

	// List of all GameObjects, Meshes, Textures and Materials used in the scene
	std::vector<GameObject*> game_objects;
	std::vector<Mesh*> meshes;
	std::map<unsigned int, Texture> scene_textures;
	std::vector<Material> scene_materials;
};

