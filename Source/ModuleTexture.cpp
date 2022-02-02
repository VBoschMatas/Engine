#include "ModuleTexture.h"
#include "IL/il.h"
#include "IL/ilu.h"
#include "SDL.h"
#include "GL/glew.h"
#include <fstream>

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

Texture ModuleTexture::LoadTexture(const char *path, bool &texture_found)
{
	Texture tex;
	tex.path = path;

	std::string n = path;
	size_t offset = n.find_last_of("\\");
	if (offset == std::string::npos)
		offset = 0;
	else ++offset;
	tex.name = n.substr(offset);

	unsigned int img_id;
	ilGenImages(1, &img_id);
	ilBindImage(img_id);
	texture_found = ilLoadImage(path);
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	std::string path_name(path);
	size_t type_pos = path_name.find_last_of('.');
	if (type_pos != std::string::npos)
	{
		std::string file_type = path_name.substr(type_pos);
		if (file_type == ".tif" || file_type == ".tiff" || file_type == ".TIF" || file_type == ".TIFF")
			iluRotate(180.0f);
	}

	// TESTGROUND

	//ILuint size;
	//ILubyte* data;
	//char** fileBuffer = new char*;
	//ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);// To pick a specific DXT compression use
	//size = ilSaveL(IL_DDS, nullptr, 0); // Get the size of the data buffer
	//if (size > 0)
	//{
	//	data = new ILubyte[size]; // allocate data buffer
	//	if (ilSaveL(IL_DDS, data, size) > 0) // Save to buffer with the ilSaveL function
	//		*fileBuffer = (char*)data;
	//}

	//std::ofstream ofs("Library\\test.dds", std::ofstream::out);

	//ofs << *fileBuffer;

	//ofs.close();

	//ilDeleteImages(1, &img_id);

	//ilGenImages(1, &img_id);
	//ilBindImage(img_id);
	//texture_found = ilLoadImage("Library\\test.dds");
	//if (texture_found) console->AddLog("FOUND!");
	//else console->AddLog("NOT FOUND!"); // ADD VAR TO TEXTS
	//ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	// TESTGROUND

	glGenTextures(1, &tex.id);
	glBindTexture(GL_TEXTURE_2D, tex.id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	tex.width = ilGetInteger(IL_IMAGE_WIDTH); tex.height = ilGetInteger(IL_IMAGE_HEIGHT);
	glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH),
		ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
		ilGetData());
	glGenerateMipmap(GL_TEXTURE_2D);

	ilDeleteImages(1, &img_id);
	return tex;
}

void ModuleTexture::UnloadTexture(int i, const unsigned int* texture)
{
	glDeleteTextures(i, texture);
}