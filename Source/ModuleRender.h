#pragma once
#include "Module.h"
#include "Globals.h"
#include "Model.h"
#include "Math/float4x4.h"
#include "MathGeoLib/Geometry/Frustum.h"

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void WindowResized(unsigned width, unsigned height);
	void* context;

private:
	unsigned int vao;
	unsigned int vbo;
	unsigned int ebo;

	unsigned int texture_id;

	unsigned int program;

	Model* model;

	unsigned int CreateVBO();
	unsigned int CreateVAO();
	unsigned int CreateEBO();
	void RenderVBOTexture(unsigned int vbo, unsigned int program);
	void DeleteVBO(unsigned int vbo);
};
