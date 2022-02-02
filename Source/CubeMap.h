#pragma once
#include "Globals.h"

class CubeMap
{
public:
	CubeMap(const float* vertices_list, std::vector<std::string> textures_path);
	~CubeMap() = default;

	unsigned int loadCubemap(std::vector<std::string> faces);

	void Update();

private:
	void initCubeMap(const float* vertices_list, std::vector<std::string> textures_path);
	unsigned int cubemapVAO, cubemapVBO;
	unsigned int cubemapTexture;
};

