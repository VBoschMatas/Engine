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

private:
	bool success;

public:
	Texture* LoadTexture(const char *path, bool &texture_found);
	void UnloadTexture(int i, const unsigned int* texture);
};

