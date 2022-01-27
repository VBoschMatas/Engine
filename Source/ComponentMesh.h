#pragma once
#include "assimp/mesh.h"
#include <vector>
#include "ModuleTexture.h"
#include "Globals.h"
#include "Component.h"
#include "ComponentMaterial.h"
#include "MathGeoLib/Geometry/AABB.h"

class Mesh
{
public:
	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, Material* material, const char* name, const std::vector<float3>& obb_vertices);
	~Mesh() = default;

	std::vector<Vertex> getVertices() { return vertices; };
	std::vector<unsigned int> getIndices() { return indices; };
	Material* getMaterial() { return material_index; };
	std::string getName() { return name; };
	unsigned int getId() { return id; };
	unsigned int getVAO() { return vao; };
	unsigned int getVerticesNum() { return num_vertices; };
	unsigned int getIndicesNum() { return num_indices; };
	math::AABB getBoundingBox() { return bounding_box; };

private:
	unsigned int id;
	std::string name;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	Material* material_index;

	math::AABB bounding_box;

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
	ComponentMesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices, Material* material, const char* name, const std::vector<float3>& obb_vertices);
	~ComponentMesh();

	void Update(unsigned int program, float3& position, Quat& rotation, float3& scale) override;

	void getBoundingBox(math::AABB& bbox) override;

	unsigned int GetVertices() { return mesh->getVerticesNum(); };
	unsigned int GetIndices() { return mesh->getIndicesNum(); };

	void printComponentInfo() override;

	bool visible;

private:
	Mesh* mesh;
};