#include "Application.h"
#include "Geometry/Frustum.h"
#include "ModuleEditorCamera.h"
#include "Scene.h"
#include "Model.h"

Scene::Scene(unsigned int _id)
{
	name = "New Scene";
	id = _id;
	last_go_id = 0;
	last_material_id = 0;
	last_mesh_id = 0;
};

Scene::Scene(const char* _name, unsigned int _id) {
	name = _name;
	id = _id;
	last_go_id = 0;
	last_material_id = 0;
	last_mesh_id = 0;
};



void Scene::Update(unsigned int program)
{
	Culling();

	for (GameObject* go : children)
	{
		go->Update(program);
	}
}

void Scene::Culling()
{
	Frustum frustum;
	if (camera_culling == nullptr)
		frustum = App->editorcamera->getFrustum();
	else
		frustum = camera_culling->getFrustum();

	for (GameObject* go : game_objects)
	{
		if (go->getBoundingBox().IsFinite() && frustum.Intersects(go->getBoundingBox()))
			go->render = true;
		else
			go->render = false;
	}
}


GameObject* Scene::AddGameObject(const std::string file_name, GoType type)
{
	return AddGameObject(file_name, nullptr, type);
}

GameObject* Scene::AddGameObject(const std::string file_name, GameObject* parent, GoType type)
{
	++last_go_id;
	GameObject* go = new GameObject(last_go_id, parent);

	go->Load(file_name, type);

	game_objects.push_back(go);

	if(parent == nullptr)
		children.push_back(go);

	return go;
}

void Scene::NewTexture(const char* path)
{
	unsigned int tex_hash = std::hash<std::string>{}(path);
	std::map<unsigned int, Texture>::iterator it = GetTextures()->find(tex_hash);
	std::map<unsigned int, Texture>::iterator end_it = GetTextures()->end();
	if (it != end_it)
	{
		return;
	}

	Texture texture;
	bool temp;
	texture = App->textures->LoadTexture(path, temp);
	AddTexture(tex_hash, texture);
}

void Scene::printHierarchy()
{

	if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{

		static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

		static int selection_mask = (1 << 2);

		for (GameObject* go : children)
		{
			go->printHierarchy(base_flags);
		}

		if (selected_gameObject != nullptr)
		{
			selected_gameObject->selected = true;
		}
	}
}