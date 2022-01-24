#pragma once
#include "Globals.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ModuleTexture.h"
#include "ComponentMaterial.h"
#include <vector>
#include <map>

class GameObject;

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

	GameObject* AddGameObject(const std::string file_name, GoType type = GoType::Empty);
	GameObject* AddGameObject(const std::string file_name, GameObject* parent, GoType type = GoType::Empty);
	void RemoveGameObject(unsigned int id);

	GameObject* AddMesh(ComponentMesh* _mesh) { meshes.push_back(_mesh); };
	void RemoveMesh(unsigned int id);

	GameObject* AddMaterial(Material _material) { scene_materials.push_back(_material); };
	void RemoveMaterial(unsigned int id);

	std::vector<GameObject*> getChildren() { return children; };

	GameObject* getGameObject();
	std::vector<GameObject*> getGameObjects() { return game_objects; };

	void printHierarchy();

	GameObject* selected_gameObject = nullptr;

private:
	unsigned int id;
	std::string name;
	std::vector<GameObject*> children; // GameObjects that are directly attached to the root
	unsigned int last_go_id;

	// List of all GameObjects, Meshes, Textures and Materials used in the scene
	std::vector<GameObject*> game_objects;
	std::vector<ComponentMesh*> meshes;
	std::map<unsigned int, Texture> scene_textures;
	std::vector<Material> scene_materials;
};

