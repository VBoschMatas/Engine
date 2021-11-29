#pragma once
#include "Module.h"
#include "Globals.h"

typedef unsigned __int8 Uint8;

class ModuleInput : public Module
{
public:
	
	ModuleInput();
	~ModuleInput();

	bool Init();
	update_status Update();
	bool CleanUp();

	// Check key states (includes mouse and joy buttons)
	const unsigned GetKey(int id) const
	{
		return keyboard[id];
	}

	const unsigned GetMouseButtonDown(int id) const
	{
		//return mouse_buttons[id - 1];
	}

private:
	const Uint8 *keyboard = NULL;
};