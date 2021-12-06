#pragma once
#include "assimp/mesh.h"
#include <vector>
#include "Globals.h"

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<unsigned int> textures);
	~Mesh() = default;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<unsigned int> textures;

	void Draw(unsigned int program);
private:
	unsigned int vbo;
	unsigned int ebo;
	unsigned int vao;

	unsigned int material_index;
	unsigned int num_vertices;
	unsigned int num_indices;

	void LoadVBO();
	void LoadEBO();
	void CreateVAO();
};

