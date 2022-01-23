#pragma once
#include "assimp/mesh.h"
#include <vector>
#include "ModuleTexture.h"
#include "Globals.h"
#include "Component.h"

class Mesh : public Component
{
public:
	Mesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, std::vector<Texture> &textures);
	~Mesh();

	void Update(unsigned int program, float3 &position, float3 &rotation, float3 &sacale) override;

	unsigned int GetVertices() { return num_vertices; };
	unsigned int GetIndices() { return num_indices; };

	void PrintComponentInfo() override;
private:
	unsigned int vbo;
	unsigned int ebo;
	unsigned int vao;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	unsigned int material_index;
	unsigned int num_vertices;
	unsigned int num_indices;

	void LoadVBO();
	void LoadEBO();
	void CreateVAO();
};

