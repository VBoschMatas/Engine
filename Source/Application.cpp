#pragma once
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleInput.h"
#include "ModuleProgram.h"
#include "ModuleRenderExercise.h"
#include "ModuleEditor.h"
#include "ModuleEditorCamera.h"
#include "ModuleTexture.h"
#include "ModuleDebugDraw.h"
#include "Timer.h"
#include "MicroTimer.h"

using namespace std;

Application::Application()
{
	timer = new Timer();
	micro_timer = new MicroTimer();
	timer->StartTime();
	micro_timer->StartTime();

	// Order matters: they will Init/start/update in this order
	modules.push_back(window = new ModuleWindow());
	modules.push_back(input = new ModuleInput());
	modules.push_back(textures = new ModuleTexture());
	modules.push_back(editorcamera = new ModuleEditorCamera());
	modules.push_back(renderer = new ModuleRender());
	//modules.push_back(rendererex = new ModuleRenderExercise());
	modules.push_back(program = new ModuleProgram());
	modules.push_back(editor = new ModuleEditor());
	modules.push_back(dd = new ModuleDebugDraw());
}

Application::~Application()
{
	for(list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
    {
        delete *it;
    }
}

bool Application::Init()
{
	bool ret = true;

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->Init();

	DEBUG("INIT TIME: ms: %d,  micro: %f", timer->ReadTime(), micro_timer->ReadTime());

	//SDL_Delay(3 * 1000); // REMOVE THIS FOR 3s OF BETTER PERFORMANCE AT LOAD

	DEBUG("POST-DELAY TIME: ms: %d,  micro: %f", timer->StopTime(), micro_timer->StopTime());
	return ret;
}

update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PreUpdate();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->Update();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PostUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for(list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
		ret = (*it)->CleanUp();

	return ret;
}
