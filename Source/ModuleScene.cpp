#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleScene.h"
#include "imgui.h"
#include <array>
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
	Scenes[current_scene]->UpdateTransform();
	Scenes[current_scene]->UpdateBoundingBox();
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

	scene->Load();
}

void ModuleScene::LoadScene(const char* path)
{
	Scene* scene = new Scene("", scenes_number);

	Scenes.push_back(scene);

	current_scene = Scenes.size() - 1;

	scene->LoadFile(path);
}

void ModuleScene::RemoveScene(unsigned int id) //for now it deletes all GameObjects
{
	Scenes.clear();
}

Scene* ModuleScene::getScene(unsigned int id)
{
	return Scenes[id];
}

void ModuleScene::Draw(unsigned int program)
{
	Scenes[current_scene]->Update(program);
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
	std::array<const char*, 14> img_formats {".png", ".jpeg",  ".jpg", ".tif", ".tiff", ".gif", ".bmp",
											 ".PNG", ".JPEG",  ".JPG", ".TIF", ".TIFF", ".GIF", ".BMP" };
	if (std::find(img_formats.begin(), img_formats.end(), file_type) != img_formats.end())
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