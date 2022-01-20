#pragma once
#include "Module.h"
#include "Globals.h"
#include <vector>
#include "Model.h"
#include "GameObject.h"

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

	void AddGameObject(const std::string file_name, GoType type);
	void RemoveGameObject(unsigned int id);

	void Draw(unsigned int program);

private:
	std::vector<Model*> GameObjects;

};

