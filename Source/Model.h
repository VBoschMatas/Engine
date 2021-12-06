#pragma once
#include "assimp/scene.h"
#include <vector>
#include "Globals.h"
#include "Mesh.h"

class Model
{
public:
	Model() = default;
	~Model() = default;

	void Load(const std::string file_name);
	void Draw(unsigned int program);
private:
	std::vector<Mesh> meshes;
	std::string directory;

	std::vector<unsigned int> LoadTextures(aiMaterial* material, aiTextureType type);
	Mesh LoadMeshes(aiMesh* mesh, const aiScene* scene);
};

