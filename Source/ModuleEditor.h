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
	//Windows
	bool console_window = true;
	bool about_window = false;
	bool configuration_window = false;

	std::vector<float> ms;
	std::vector<float> fps;
	bool quit_engine = false;
	std::string license_text;

	Timer print_freq;

	void MainMenuBar();
	void AboutWindow();
	void ConsoleWindow();
	void ConfigurationWindow();

	void VirtualMemory(float& total, float& consumed);
	void PhysicalMemory(float& total, float& consumed);
};
