#include "Scene.h"
#include "Model.h"







void Scene::AddGameObject(const std::string file_name, GoType type)
{
	++last_go_id;
	GameObject* model = new GameObject(last_go_id);

	//model->Model::Load(file_name);
	
	switch (type)
	{
	case GoType::Model:
		//Model* model = new Model(last_go_id);
		break;
	default:
		//GameObject* go = new GameObject(last_go_id);
		break;
	}

	children.push_back(model);
}