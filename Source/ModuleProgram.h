#pragma once
#include "Module.h"
#include "Globals.h"
#include "GL/glew.h"


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

	GLuint program;
	GLuint outline_program;
	GLuint skybox_program;

	GLuint CreateProgram(const char* source_1, const char* source_2);
	GLuint CreateOutlineProgram(const char* source_1, const char* source_2);
	GLuint CreateSkyboxProgram(const char* source_1, const char* source_2);

private:
	void* context;

private:
	GLuint CompileShader(GLuint type, const char* source);
	char* LoadSource(const char* source_file);
	GLuint CreateProgram(GLuint vtx_shader, GLuint frg_shader);
};
