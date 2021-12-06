#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleProgram.h"
#include "ModuleTexture.h"
#include "GL/glew.h"
#include "IL/il.h"
#include "SDL.h"
#include "MathGeoLib.h"
#include "ModuleEditorCamera.h"
#include "ModuleDebugDraw.h"
#include "Model.h"

ModuleRender::ModuleRender()
{
	
}

// Destructor
ModuleRender::~ModuleRender()
{
}

void __stdcall OurOpenGLErrorFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	const char* tmp_source = "", * tmp_type = "", * tmp_severity = "";
	switch (source) {
	case GL_DEBUG_SOURCE_API: tmp_source = "API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: tmp_source = "Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: tmp_source = "Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY: tmp_source = "Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION: tmp_source = "Application"; break;
	case GL_DEBUG_SOURCE_OTHER: tmp_source = "Other"; break;
	};
	switch (type) {
	case GL_DEBUG_TYPE_ERROR: tmp_type = "Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: tmp_type = "Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: tmp_type = "Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY: tmp_type = "Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE: tmp_type = "Performance"; break;
	case GL_DEBUG_TYPE_MARKER: tmp_type = "Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP: tmp_type = "Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP: tmp_type = "Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER: tmp_type = "Other"; break;
	};
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: tmp_severity = "high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM: tmp_severity = "medium"; break;
	case GL_DEBUG_SEVERITY_LOW: tmp_severity = "low"; break;
		// case GL_DEBUG_SEVERITY_NOTIFICATION: tmp_severity = "notification"; break;
	default: return;
	};
	DEBUG("<Source:%s> <Type:%s> <Severity:%s> <ID:%d> <Message:%s>\n", tmp_source, tmp_type, tmp_severity, id, message);
}

// Called before render is available
bool ModuleRender::Init()
{
	DEBUG("Creating Renderer context");
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	context = SDL_GL_CreateContext(App->window->window);

	glewInit();
	
	DEBUG("Using Glew %s", glewGetString(GLEW_VERSION));

	DEBUG("Vendor: %s", glGetString(GL_VENDOR));
	DEBUG("Renderer: %s", glGetString(GL_RENDERER));
	DEBUG("OpenGL version supported %s", glGetString(GL_VERSION));
	DEBUG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	DEBUG("Gl Options Initialized");

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(&OurOpenGLErrorFunction, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);

	// Textures params
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	SDL_Surface* screen_surface = App->window->screen_surface;
	glViewport(0, 0, screen_surface->w, screen_surface->h);

	program = App->program->CreateProgram("shaders/texture_vertex.glsl", "shaders/texture_fragment.glsl");

	//vbo = CreateVBO();

	// Generate

	//unsigned int img_id = App->textures->LoadTexture("textures/Lenna.png");

	

	//glUseProgram(program);
	//glUniform1i(glGetUniformLocation(program, "mytexture"), 0);

	//ilDeleteImages(1, &img_id);

	model = new Model();
	model->Load("BakerHouse.fbx");

	return true;
}

update_status ModuleRender::PreUpdate()
{
	SDL_Surface* screen_surface = App->window->screen_surface;
	glViewport(0, 0, screen_surface->w, screen_surface->h);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update()
{
	SDL_Surface* screen_surface = App->window->screen_surface;
	//RenderVBOTexture(vbo, program);

	App->dd->Draw(App->editorcamera->getView(), App->editorcamera->getProjection(), screen_surface->w, screen_surface->h);

	model->Draw(program);

	//RenderVBO(vbo, program);

	return UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate()
{
	SDL_GL_SwapWindow(App -> window->window);
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{

	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);

	DEBUG("Destroying renderer");
	SDL_GL_DeleteContext(context);
	//Destroy window

	return true;
}

void ModuleRender::WindowResized(unsigned width, unsigned height)
{
	SDL_UpdateWindowSurface(App->window->window);
	App->window->screen_surface = SDL_GetWindowSurface(App->window->window);
}

unsigned int ModuleRender::CreateVBO()
{
	float vertices[] = {
		//Quad					//Texture
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f 
	};

	unsigned int vbo;

	vao = CreateVAO();

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	ebo = CreateEBO();

	return vbo;
}

unsigned int ModuleRender::CreateVAO()
{
	unsigned int vao;

	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);

	return vao;
}

unsigned int ModuleRender::CreateEBO()
{
	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	unsigned int ebo;

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	return ebo;
}

void ModuleRender::DeleteVBO(unsigned int vbo)
{
	glDeleteBuffers(1, &vbo);
}

void ModuleRender::RenderVBOTexture(unsigned int vbo, unsigned int program)
{
	glUseProgram(App->program->program);

	float4x4 model = float4x4::identity;

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, &App->editorcamera->getView()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, &App->editorcamera->getProjection()[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	glBindVertexArray(vao);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}