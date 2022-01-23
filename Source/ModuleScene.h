#pragma once
#include "Module.h"
#include "Globals.h"
#include <vector>
#include "Model.h"
#include "Scene.h"

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
	void Update(unsigned int program);

	unsigned int current_scene;

private:
	unsigned int scenes_number;
	std::vector<Scene*> Scenes;

};

