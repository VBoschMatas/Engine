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
class ModuleEditor;
class ModuleEditorCamera;
class ModuleTexture;
class MicroTimer;
class Timer;

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
	ModuleEditor* editor = nullptr;
	ModuleEditorCamera* editorcamera = nullptr;
	ModuleTexture* textures = nullptr;

private:
	Timer* timer = nullptr;
	MicroTimer* micro_timer = nullptr;
	std::list<Module*> modules;

};

extern Application* App;
