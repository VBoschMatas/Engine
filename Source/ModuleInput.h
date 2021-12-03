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
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	void CenterMouse();

	// Check key states (includes mouse and joy buttons)
	const unsigned GetKey(int id) const
	{
		return keyboard[id];
	}

	const unsigned GetMouseButton(int id) const
	{
		return mouse_buttons;
	}

	const void GetMouseMovement(int &_x, int &_y) const
	{
		_x = mouse_motion_x;
		_y = mouse_motion_y;
	}

private:
	const Uint8 *keyboard = NULL;
	Uint32 mouse_buttons;
	int mouse_motion_x, mouse_motion_y;
	int mouse_x, mouse_y;
};