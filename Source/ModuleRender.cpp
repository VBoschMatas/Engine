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

	App->dd->Draw(App->editorcamera->getView(), App->editorcamera->getProjection(), screen_surface->w, screen_surface->h);
	if(model != nullptr)
		model->Draw(program);

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
	DEBUG("Destroying renderer");
	SDL_GL_DeleteContext(context);

	delete(model);

	return true;
}

void ModuleRender::DroppedModel(char* path)
{
	std::string path_name(path);
	if (path_name.substr(path_name.find_last_of('.')) != ".fbx" && path_name.substr(path_name.find_last_of('.')) != ".FBX")
	{
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_INFORMATION,
			"File dropped has an incorrect format",
			"The dropped file has to be a \".fbx\"",
			App->window->window
		);
		return;
	}
	if (model != nullptr)
		delete(model);

	model = new Model();
	model->Load(path_name);
}


void ModuleRender::WindowResized(unsigned width, unsigned height)
{
	SDL_UpdateWindowSurface(App->window->window);
	App->window->screen_surface = SDL_GetWindowSurface(App->window->window);
}