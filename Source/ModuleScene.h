#pragma once
#include "Module.h"
#include "Globals.h"
#include <vector>
#include "Model.h"
#include "Scene.h"
#include "EngineQuadTree.h"

class ComponentCamera;
class EngineQuadtree;

class ModuleScene : public Module
{
public:

	ModuleScene();

	// Destructor
	virtual ~ModuleScene();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	void AddScene(const char* name);
	void RemoveScene(unsigned int id);
	Scene* getScene(unsigned int id);
	//Scene* getCurrentScene() { return getScene(current_scene); };
	void Draw(unsigned int program);

	void printHierarchy();

	void DroppedModel(char* path);

	unsigned int current_scene;

private:
	unsigned int scenes_number;
	std::vector<Scene*> Scenes;

public: // Functions that will act on the current scene, used to simplify calls from other classes
	GameObject* AddGameObject(const std::string file_name, GoType type = GoType::Empty) const
	{
		return AddGameObject(file_name, nullptr, type);
	};
	GameObject* AddGameObject(const std::string file_name, GameObject* parent, GoType type = GoType::Empty) const
	{
		return Scenes[current_scene]->AddGameObject(file_name, parent, type);
	};
	void AddGameObjectIntoQuadtree(GameObject* gameobject) const
	{
		Scenes[current_scene]->AddGameObjectIntoQuadtree(gameobject);
	};
	void RemoveGameObject(GameObject* gameobj) const 
	{
		Scenes[current_scene]->RemoveGameObject(gameobj);
	};
	void RemoveGameObjectFromQuadtree(GameObject* gameobj) const
	{
		Scenes[current_scene]->RemoveGameObjectFromQuadtree(gameobj);
	};
	void AddMesh(Mesh* _mesh) const
	{
		Scenes[current_scene]->AddMesh(_mesh);
	};
	std::vector<Mesh*> GetMeshes() const 
	{
		return Scenes[current_scene]->GetMeshes();
	};
	void RemoveMesh(unsigned int id) const
	{
		Scenes[current_scene]->RemoveMesh(id);
	};
	Material* AddMaterial(Material* _material) const
	{ 
		return Scenes[current_scene]->AddMaterial(_material);
	};
	std::vector<Material*> GetMaterials() const
	{
		return Scenes[current_scene]->GetMaterials();
	};
	void RemoveMaterial(unsigned int id) const
	{
		Scenes[current_scene]->RemoveMaterial(id);
	};
	void NewTexture(const char* path) const
	{
		Scenes[current_scene]->NewTexture(path);
	};
	Texture* AddTexture(unsigned int index, Texture texture) const
	{
		return Scenes[current_scene]->AddTexture(index, texture);
	};
	std::map<unsigned int, Texture>* GetTextures() const
	{ 
		return  Scenes[current_scene]->GetTextures(); 
	};
	void RemoveTexture(unsigned int id) const
	{
		Scenes[current_scene]->RemoveTexture(id);
	};
	std::vector<GameObject*> getChildren() const 
	{ 
		return  Scenes[current_scene]->getChildren(); 
	};
	GameObject* getGameObject();
	GameObject* getSelectedGameObject() const
	{
		return  Scenes[current_scene]->selected_gameObject;
	};
	void setSelectedGameObject(GameObject* gameobject)
	{
		Scenes[current_scene]->selected_gameObject = gameobject;
	};
	std::vector<GameObject*> getGameObjects() const 
	{ 
		return Scenes[current_scene]->getGameObjects(); 
	};
	ComponentCamera* getCameraCulling() const
	{
		return Scenes[current_scene]->camera_culling;
	}
	void setCameraCulling(ComponentCamera* camera)
	{
		Scenes[current_scene]->camera_culling = camera;
	}
	EngineQuadtree* getQuadtree() const
	{
		return &Scenes[current_scene]->quadtree;
	}
	EngineQuadtree* setQuadtree(EngineQuadtree _quadtree) const
	{
		Scenes[current_scene]->quadtree = _quadtree;
	}
	void AmbientLightInfo() const
	{
		Scenes[current_scene]->AmbientLightInfo();
	}

	unsigned int getGoId() const { return Scenes[current_scene]->getGoId(); };
	unsigned int getMeshId() const { return Scenes[current_scene]->getMeshId(); };
	unsigned int getMaterialId() const { return Scenes[current_scene]->getMaterialId(); };
};

