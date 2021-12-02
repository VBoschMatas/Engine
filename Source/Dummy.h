#pragma once
#include "Module.h"
#include "Globals.h"

class ModuleDummy : public Module
{
	bool Init()
	{
		DEBUG("Dummy Init!");
		return true;
	}

	bool CleanUp()
	{
		DEBUG("Dummy CleanUp!");
		return true;
	}
};