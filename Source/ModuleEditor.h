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

	void MenuBar(const char** items);
	void MenuOption(const char* option);
};
