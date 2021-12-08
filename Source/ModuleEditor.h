#pragma once
#include "Module.h"
#include "Globals.h"
#include "SDL.h"

class ModuleEditor : public Module
{
public:
	ModuleEditor();
	~ModuleEditor();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

private:
	bool console_window = false, performance_window = false, about_window = false;
	std::vector<float> ms;
	std::vector<float> fps;
	bool quit_engine = false;
	void MainMenuBar();
	void AboutWindow();
	void PerformanceWindow();
	void ConsoleWindow();
};
