#pragma once
#include "assimp/mesh.h"
#include <vector>

class Mesh
{
public:
	Mesh();
	~Mesh();
	void Load();
	void LoadVBO(const aiMesh* mesh);
	void LoadEBO(const aiMesh* mesh);
	void CreateVAO();
	void Draw(const std::vector<unsigned int>& model_textures);
private:
	unsigned int vbo;
	unsigned int ebo;
	unsigned int vao;

	unsigned int num_vertices;
	unsigned int num_indices;
};

