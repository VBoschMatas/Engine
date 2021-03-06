#pragma once
#include "Module.h"
#include "Globals.h"

enum class TexType
{
	diffusse,
	specular,
	normal
};

struct Texture
{
	std::string path;
	std::string name;
	unsigned int width;
	unsigned int height;
	unsigned int id;
	TexType type;
};

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

public:
	Texture LoadTexture(const char *path, bool &texture_found);
	void UnloadTexture(int i, const unsigned int* texture);
};

