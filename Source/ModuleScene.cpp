#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleScene.h"
#include "imgui.h"
#include "SDL.h"

ModuleScene::ModuleScene()
{
	Scenes = {};
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
	Scenes = {};
	current_scene = 0;

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
	for (int i = 0; i < Scenes.size(); ++i)
	{
		delete(Scenes[i]);
	}

	Scenes.clear();
	return true;
}

void ModuleScene::AddScene(const char* name)
{
	Scene* scene = new Scene(name, scenes_number);

	Scenes.push_back(scene);
}

void ModuleScene::RemoveScene(unsigned int id) //for now it deletes all GameObjects
{
	Scenes.clear();
}

Scene* ModuleScene::getScene(unsigned int id)
{
	return Scenes[id];
}

void ModuleScene::Update(unsigned int program)
{
	Scenes[current_scene]->Update(program);
	/*for (int i = 0; i < Scenes.size(); ++i)
	{
		Scenes[i]->Update(program);
	}*/
}

void ModuleScene::DroppedModel(char* path)
{
	std::string path_name(path);
	std::string file_type = path_name.substr(path_name.find_last_of('.'));
	if (file_type == ".fbx" || file_type == ".FBX")
	{
		getScene(App->scene->current_scene)->AddGameObject(path_name, GoType::Model);
		return;
	}

	if (file_type == ".png" || file_type == ".jpeg" || file_type == ".jpg" ||
		file_type == ".PNG" || file_type == ".JPEG" || file_type == ".JPG")
	{
		getScene(App->scene->current_scene)->NewTexture(path);
		return;
	}


	SDL_ShowSimpleMessageBox(
		SDL_MESSAGEBOX_INFORMATION,
		"Import error",
		"The dropped file does not have an accepted format",
		App->window->window
	);
	//App->editorcamera->FitNewModel();
}

void ModuleScene::printHierarchy()
{
	Scenes[current_scene]->printHierarchy();
}