#include "ModuleScene.h"

ModuleScene::ModuleScene()
{
	GameObjects = {};
}

ModuleScene::~ModuleScene()
{
	/*for (int i = 0; i < GameObjects.size(); ++i)
	{
		delete(GameObjects);
	}*/
}

bool ModuleScene::Init()
{
	GameObjects = {};

	return true;
}

update_status ModuleScene::PreUpdate()
{
	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update()
{
	return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate()
{
	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	for (int i = 0; i < GameObjects.size(); ++i)
	{
		delete(GameObjects[i]);
	}

	GameObjects.clear();
	return true;
}

void ModuleScene::AddGameObject(const std::string file_name, GoType type)
{
	Model* model = new Model();
	model->Load(file_name);

	GameObjects.push_back(model);
}

void ModuleScene::RemoveGameObject(unsigned int id) //for now it deletes all GameObjects
{
	GameObjects.clear();
}

void ModuleScene::Draw(unsigned int program)
{
	for (int i = 0; i < GameObjects.size(); ++i)
	{
		GameObjects[i]->Draw(program);
	}
}
