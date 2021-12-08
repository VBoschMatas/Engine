#pragma once
#include "Module.h"
#include "Globals.h"

#define NUM_MOUSE_BUTTONS 5

typedef unsigned __int8 Uint8;
typedef unsigned __int32 Uint32;

enum class KeyState
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class ModuleInput : public Module
{
public:
	
	ModuleInput();
	~ModuleInput();

	bool Init();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	// Check key states (includes mouse and joy buttons)
	const unsigned GetKey(int id) const
	{
		return keyboard[id];
	}

	const bool GetPressedKey(unsigned int id) const
	{
		return kybrd_button == id;
	}

	const bool GetMouseButton(int id) const
	{
		return mouse_buttons == id;
	}

	const void GetMouseMovement(int &_x, int &_y) const
	{
		_x = mouse_motion_x;
		_y = mouse_motion_y;
	}

	int GetMouseWheel() const
	{
		return mouse_wheel;
	}

private:
	char* dropped_filedir;
	const Uint8 *keyboard = NULL;
	Uint32 mouse_buttons;
	Uint8 kybrd_button;
	int mouse_wheel;
	int mouse_motion_x, mouse_motion_y;
	int mouse_x, mouse_y;
};