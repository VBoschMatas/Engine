#pragma once

#include<list>
#include "Globals.h"
#include "Module.h"

class ModuleProgram;
class ModuleRenderExercise;
class ModuleRender;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;

class Application
{
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

public:
	ModuleProgram* program = nullptr;
	ModuleRenderExercise* rendererex = nullptr;
	ModuleRender* renderer = nullptr;
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;

private:

	std::list<Module*> modules;

};

extern Application* App;
