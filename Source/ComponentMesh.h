#pragma once
#include "assimp/mesh.h"
#include <vector>
#include "ModuleTexture.h"
#include "Globals.h"
#include "Component.h"
#include "ComponentMaterial.h"

class Mesh
{
public:
	Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures, std::string name);
	~Mesh() = default;

	std::vector<Vertex> getVertices() { return vertices; };
	std::vector<unsigned int> getIndices() { return indices; };
	Material* getMaterial() { return material_index; };
	std::string getName() { return name; };
	unsigned int getId() { return id; };
	unsigned int getVAO() { return vao; };
	unsigned int getVerticesNum() { return num_vertices; };
	unsigned int getIndicesNum() { return num_indices; };

private:
	unsigned int id;
	std::string name;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	Material* material_index;

	unsigned int vbo;
	unsigned int ebo;
	unsigned int vao;

	unsigned int num_vertices;
	unsigned int num_indices;

	void LoadVBO();
	void LoadEBO();
	void CreateVAO();
};

class ComponentMesh : public Component
{
public:
	ComponentMesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, std::vector<Texture> &textures);
	~ComponentMesh();

	void Update(unsigned int program, float3& position, Quat& rotation, float3& scale) override;

	unsigned int GetVertices() { return mesh->getVerticesNum(); };
	unsigned int GetIndices() { return mesh->getIndicesNum(); };

	void printComponentInfo() override;

	bool visible;

private:
	Mesh* mesh;
};