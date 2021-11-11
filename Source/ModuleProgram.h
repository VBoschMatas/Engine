#pragma once
#include "Module.h"
#include "Globals.h"
#include "glew-2.1.0/include/GL/glew.h"

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleProgram : public Module
{
public:
	ModuleProgram();
	~ModuleProgram();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	char* LoadSource(const char* source_file);

private:
	GLuint shader_1, shader_2;
	void* context;
	float vertices[];

private:
	unsigned CreateVBO();
	void RenderVBO(unsigned vbo);
};
