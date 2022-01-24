#pragma once
#include "assimp/mesh.h"
#include <vector>
#include "ModuleTexture.h"
#include "Globals.h"
#include "Component.h"

//struct Mesh
//{
//	unsigned int id;
//};

class ComponentMesh : public Component
{
public:
	ComponentMesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, std::vector<Texture> &textures);
	~ComponentMesh();

	void Update(unsigned int program, float3& position, Quat& rotation, float3& scale) override;

	unsigned int GetVertices() { return num_vertices; };
	unsigned int GetIndices() { return num_indices; };

	void printComponentInfo() override;

	bool visible;
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

