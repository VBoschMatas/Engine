#pragma once
#include "Module.h"
#include "Globals.h"
#include "GL/glew.h"

class ModuleRenderExercise : public Module
{
public:
	ModuleRenderExercise();
	~ModuleRenderExercise();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

private:
	GLuint vbo_tris;
	GLuint program_hello_world;

	GLuint CreateVBO();
	void RenderVBO(GLuint vbo, GLuint program);
	void RenderVBOTexture(GLuint vbo, GLuint program);
	void DeleteVBO(GLuint vbo);
};

