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
	void GetHardware(char*& vendor, char*& renderer, char*& opengl, char*& glsl, char*& glew);
	void LoadFrameBuffer();
	void ClearFrameBuffer();

	void TempLight();

	void* context;
	Model* model;

	unsigned int GetFBTexture() { return textureColorbuffer; };

private:
	unsigned int texture_id;

	unsigned int fbo;
	unsigned int textureColorbuffer;

	unsigned int program;

	ImVec2 currentWindowSize;
};
