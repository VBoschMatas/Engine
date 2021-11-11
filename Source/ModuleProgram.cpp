#include "Globals.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleRender.h"
#include "SDL/include/SDL.h"


ModuleProgram::ModuleProgram()
{}

// Destructor
ModuleProgram::~ModuleProgram()
{}

// Called before render is available
bool ModuleProgram::Init()
{
	//shader_1 = glCreateShader(GL_VERTEX_SHADER);
	//shader_2 = glCreateShader(GL_FRAGMENT_SHADER);

	unsigned vbo_triangle = CreateVBO();



	//RenderVBO(vbo_triangle);

	//LoadSource();
	return true;
}

update_status ModuleProgram::PreUpdate()
{
	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleProgram::Update()
{
	return UPDATE_CONTINUE;
}

update_status ModuleProgram::PostUpdate()
{
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleProgram::CleanUp()
{
	return true;
}

unsigned ModuleProgram::CreateVBO()
{
	float vertices[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f
	};
	unsigned vbo;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	return vbo;
}

void ModuleProgram::RenderVBO(unsigned vbo)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_TRIANGLES, 0, 3);
}

char* ModuleProgram::LoadSource(const char* file_name)
{
	char* data = nullptr;
	FILE* file = nullptr;
	fopen_s(&file, file_name, "rb");
	if (file)
	{
		fseek(file, 0, SEEK_END);
		int size = ftell(file);
		data = (char*)malloc(size + 1);
		fseek(file, 0, SEEK_SET);
		fread(data, 1, size, file);
		data[size] = 0;
		fclose(file);
	}
	return data;
}