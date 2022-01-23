#include "Scene.h"
#include "Model.h"





void Scene::Update(unsigned int program)
{
	for (GameObject* go : children)
	{
		go->Update(program);
	}
}


void Scene::AddGameObject(const std::string file_name, GoType type)
{
	++last_go_id;
	GameObject* go = new GameObject(last_go_id);

	go->Load(file_name, type);

	game_objects.push_back(go);
	children.push_back(go);
}