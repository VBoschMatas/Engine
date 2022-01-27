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
#include "ModuleScene.h"
#include "GameObject.h"
#include "Model.h"
#include "optick.h"

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
	console->AddLog("<Source:%s> <Type:%s> <Severity:%s> <ID:%d> <Message:%s>\n", tmp_source, tmp_type, tmp_severity, id, message);
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

	LoadFrameBuffer();

	program = App->program->CreateProgram("shaders/light_vertex.glsl", "shaders/light_fragment.glsl");

	//model = new Model();
	//model->Load("BakerHouse.fbx");

	App->scene->AddScene("Scene 1");

	App->scene->getScene(0)->AddGameObject("BakerHouse.fbx", GoType::Model);
	//App->scene->getScene(0)->AddGameObject("models/WoodenCrate01.fbx", GoType::Model);

	//App->scene->AddGameObject(GoType::Model, "models/WoodenCrate01.fbx");
	//App->scene->AddGameObject(GoType::Model, "BakerHouse.fbx");
	return true;
}

update_status ModuleRender::PreUpdate()
{
	SDL_Surface* screen_surface = App->window->screen_surface;
	glViewport(0, 0, screen_surface->w, screen_surface->h);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ClearFrameBuffer();
	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update()
{
	SDL_Surface* screen_surface = App->window->screen_surface;

	//TempLight();

	//Draw inside the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	App->scene->Update(program);
	/*if(model != nullptr)
		model->Draw(program);*/

	App->dd->Draw(App->editorcamera->getView(), App->editorcamera->getProjection(), screen_surface->w, screen_surface->h);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate()
{
	OPTICK_EVENT();
	SDL_GL_SwapWindow(App -> window->window);
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	DEBUG("Destroying renderer");
	SDL_GL_DeleteContext(context);
	glDeleteFramebuffers(1, &fbo);

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

	App->scene->getScene(App->scene->current_scene)->AddGameObject(path_name, GoType::Model);
	//App->editorcamera->FitNewModel();
}

void ModuleRender::WindowResized(unsigned width, unsigned height)
{
	SDL_UpdateWindowSurface(App->window->window);
	App->window->screen_surface = SDL_GetWindowSurface(App->window->window);
}

void ModuleRender::GetHardware(char*& vendor, char*& renderer, char*& opengl, char*& glsl, char*& glew)
{
	vendor = (char*) glGetString(GL_VENDOR);
	renderer = (char*) glGetString(GL_RENDERER);
	opengl = (char*) glGetString(GL_VERSION);
	glsl = (char*) glGetString(GL_SHADING_LANGUAGE_VERSION);
	glew = (char*) glewGetString(GLEW_VERSION);
}

void ModuleRender::TempLight()
{
	glUniform3f(glGetUniformLocation(program, "lightDir"), 0.2f, 0.9f, 0.7f);
	glUniform3f(glGetUniformLocation(program, "ambientColor"), 1.0f, 1.0f, 1.0f);
}

void ModuleRender::LoadFrameBuffer()
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// We create the frameobject texture
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, App->window->screen_surface->w, App->window->screen_surface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

	// We create a Renderbuffer to save the data in a quick access buffer
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, App->window->screen_surface->w, App->window->screen_surface->h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

}

void ModuleRender::ClearFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, App->window->screen_surface->w, App->window->screen_surface->h);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}