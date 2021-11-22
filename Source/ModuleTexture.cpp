#include "ModuleTexture.h"
#include "IL/il.h"
#include "IL/ilu.h"
#include "SDL.h"
#include "GL/glew.h"

ModuleTexture::ModuleTexture()
{

}

ModuleTexture::~ModuleTexture()
{

}

bool ModuleTexture::Init()
{
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	{
		/* wrong DevIL version */
		SDL_Quit();
		return -1;
	}

	ilInit();

	return true;
}

update_status ModuleTexture::PreUpdate()
{
	return UPDATE_CONTINUE;
}

update_status ModuleTexture::Update()
{
	return UPDATE_CONTINUE;
}

update_status ModuleTexture::PostUpdate()
{
	return UPDATE_CONTINUE;
}

bool ModuleTexture::CleanUp()
{
	return true;
}

unsigned int ModuleTexture::LoadTexture(const char *path)
{
	unsigned int texid;
	ilGenImages(1, &texid);
	ilBindImage(texid);
	ilLoadImage(path);
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	iluRotate(180.0f);

	return texid;
}

void ModuleTexture::UnloadTexture(int i, const unsigned int* texture)
{
	ilDeleteImages(i, texture);
}