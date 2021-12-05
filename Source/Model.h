#pragma once
#include "assimp/scene.h"
#include <vector>

class Model
{
public:
	Model() = default;
	~Model() = default;

	void Load(const char* file_name);
	void LoadMaterials(const aiScene* scene);
private:
	std::vector<unsigned int> materials;
};

