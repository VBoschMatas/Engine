#pragma once
#include "Module.h"
#include "GameObject.h"
#include "Globals.h"
#include <vector>
#include "Model.h"

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

	void AddGameObject(GoType type, const std::string file_name);
	void RemoveGameObject(unsigned int id);

	void Draw(unsigned int program);

private:
	std::vector<Model*> GameObjects;

};

