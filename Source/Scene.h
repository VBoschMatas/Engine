#pragma once
#include "Globals.h"
#include "GameObject.h"
#include "AmbientLight.h"
#include "ComponentMesh.h"
#include "ModuleTexture.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"
#include "EngineQuadTree.h"
#include <vector>
#include <map>

class GameObject;
class Skybox;

class Scene
{
public:
	Scene(unsigned int _id);
	Scene(const char* _name, unsigned int _id);
	~Scene() {};

	void setName(const char* _name) { name = _name; };

	void Load();
	void UpdateTransform();
	void UpdateBoundingBox();
	void UpdateLights(unsigned int program);
	void Update(unsigned int program);

	unsigned int getId() { return id; };
	std::string getName() { return name; };

	GameObject* AddGameObject(const std::string file_name, GoType type = GoType::Empty);
	GameObject* AddGameObject(const std::string file_name, GameObject* parent, GoType type = GoType::Empty);
	void AddGameObjectIntoQuadtree(GameObject* gameobject);
	void RemoveGameObject(GameObject* gameobj)
	{
		game_objects.erase(std::find(game_objects.begin(), game_objects.end(), gameobj));
		children.erase(std::find(children.begin(), children.end(), gameobj));
		delete(gameobj);
	}
	void RemoveGameObjectFromQuadtree(GameObject* gameobj);

	void AddMesh(Mesh* _mesh) { meshes.push_back(_mesh); };
	std::vector<Mesh*> GetMeshes() { return meshes; };
	void RemoveMesh(unsigned int id);

	Material* AddMaterial(Material* _material) { scene_materials.push_back(_material); return scene_materials.back(); };
	std::vector<Material*> GetMaterials() { return scene_materials; };
	void RemoveMaterial(unsigned int id);

	void NewTexture(const char* path);
	Texture* AddTexture(unsigned int index, Texture texture)
	{ 
		scene_textures.insert(std::pair<unsigned int, Texture>(index, texture));
		return &scene_textures.find(index)->second;
	};
	std::map<unsigned int, Texture>* GetTextures() { return &scene_textures; };
	void RemoveTexture(unsigned int id);

	std::vector<GameObject*> getChildren() { return children; };
	void addChild(GameObject* _gameobject) { children.push_back(_gameobject); };
	void removeChild(const GameObject* _gameobject)
	{
		children.erase(std::find(children.begin(), children.end(), _gameobject));
	}

	GameObject* getGameObject(unsigned int id);
	std::vector<GameObject*> getGameObjects() { return game_objects; };

	void printHierarchy();

	void AmbientLightInfo()
	{
		ambient_light->printGameObjectInfo();
	}

	void DrawSkybox();

	GameObject* selected_gameObject = nullptr;
	ComponentCamera* camera_culling = nullptr;
	EngineQuadtree quadtree;

	unsigned int getGoId() { unsigned int temp_id = last_go_id; ++last_go_id; return temp_id; };
	unsigned int getMeshId() { unsigned int temp_id = last_mesh_id; ++last_mesh_id; return temp_id; };
	unsigned int getMaterialId() { unsigned int temp_id = last_material_id; ++last_material_id; return temp_id; };
	unsigned int getCurrentPointLight() { unsigned int temp_id = current_pointlight; ++current_pointlight; return temp_id; }
	unsigned int getCurrentSpotLight() { unsigned int temp_id = current_spotlight; ++current_spotlight; return temp_id; }


	unsigned int last_pointlight_num = 0; // How many lights there are
	unsigned int current_pointlight = 0; // Current light updating
	unsigned int last_spotlight_num = 0;
	unsigned int current_spotlight = 0;

private:
	unsigned int id;
	std::string name;
	std::vector<GameObject*> children; // GameObjects that are directly attached to the root

	// Resources used in the scene
	AmbientLight* ambient_light;
	Skybox* skybox;
	std::vector<GameObject*> game_objects;
	std::vector<Mesh*> meshes;
	std::map<unsigned int, Texture> scene_textures;
	std::vector<Material*> scene_materials;

	unsigned int last_go_id;
	unsigned int last_mesh_id;
	unsigned int last_material_id;

	void Culling();
	void GenerateQuadtree();
};

