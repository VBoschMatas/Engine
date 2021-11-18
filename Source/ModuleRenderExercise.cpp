#include "ModuleRenderExercise.h"
#include "ModuleProgram.h"
#include "Application.h"
#include "GL/glew.h"

ModuleRenderExercise::ModuleRenderExercise()
{

}

ModuleRenderExercise::~ModuleRenderExercise()
{

}

bool ModuleRenderExercise::Init()
{
	try {
		vbo_tris = CreateVBO();
		//gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);

		program_hello_world = App->program->CreateProgram("shaders/default_vertex.glsl", "shaders/default_fragment.glsl");
	}
	catch (const char* e) {
		return false;
	}

	return true;
}

update_status ModuleRenderExercise::PreUpdate()
{
	return UPDATE_CONTINUE;
}

update_status ModuleRenderExercise::Update() 
{
	try {
		RenderVBO(vbo_tris, program_hello_world);
	}
	catch (const char* e) {
		return UPDATE_STOP;
	}
	return UPDATE_CONTINUE;
}

update_status ModuleRenderExercise::PostUpdate()
{
	return UPDATE_CONTINUE;
}

bool ModuleRenderExercise::CleanUp()
{
	return true;
}

GLuint ModuleRenderExercise::CreateVBO()
{
	float vertices[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, //text
		1.0f, 0.0f, //text
		0.5f, 1.0f  //text
	};

	GLuint vbo;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	return vbo;
}

void ModuleRenderExercise::DeleteVBO(GLuint vbo)
{
	glDeleteBuffers(1, &vbo);
}

void ModuleRenderExercise::RenderVBO(GLuint vbo, GLuint program)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glUseProgram(program);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void ModuleRenderExercise::RenderVBOTexture(GLuint vbo, GLuint program)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float)*3*3));

	glUseProgram(program);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}