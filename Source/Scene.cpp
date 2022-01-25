#include "Scene.h"
#include "Model.h"

Scene::Scene(unsigned int _id)
{
	name = "New Scene";
	id = _id;
	last_go_id = 0;
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
	for (GameObject* go : children)
	{
		go->Update(program);
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

void Scene::printHierarchy()
{

	if (ImGui::CollapsingHeader(name.c_str()))
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