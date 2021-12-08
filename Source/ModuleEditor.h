#pragma once
#include "Module.h"
#include "Globals.h"
#include "SDL.h"

#define MENU_OPTIONS 3

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
	const char* menu_items[MENU_OPTIONS] = { "File", "Edit", "View" };
	bool console_window = false, performance_window = false, about_window = false;
	std::vector<float> ms;
	std::vector<float> fps;
	bool quit_engine = false;
	void MainMenuBar();
	void MenuBar(const char** items);
	void AboutWindow();
	void PerformanceWindow();
	void ConsoleWindow();
};
