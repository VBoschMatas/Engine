#pragma once
#include "Module.h"
#include "Globals.h"

class ModuleTexture: public Module
{
public:
	ModuleTexture();
	~ModuleTexture();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

private:
	unsigned int texid;
	bool success;

public:
	unsigned int LoadTexture(const char *path, bool &texture_found);
	void UnloadTexture(int i, const unsigned int* texture);
	unsigned int getTexId() { return texid; };
};

