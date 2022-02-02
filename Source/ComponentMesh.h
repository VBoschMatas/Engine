#pragma once
#include "assimp/mesh.h"
#include <vector>
#include "ModuleTexture.h"
#include "Globals.h"
#include "Component.h"
#include "ComponentMaterial.h"
#include "MathGeoLib/Geometry/AABB.h"
#include "Geometry/Triangle.h"

class Mesh
{
public:
	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, Material* material, const char* name, const std::vector<float3>& obb_vertices);
	Mesh();
	~Mesh() = default;

	std::vector<Vertex> getVertices() { return vertices; };
	std::vector<unsigned int> getIndices() { return indices; };
	std::vector<math::Triangle> getTriangles();
	Material* getMaterial() { return material_index; };
	void setMaterial(Material* _material) { material_index = _material; };
	std::string getName() { return name; };
	unsigned int getId() { return id; };
	unsigned int getVAO() { return vao; };
	unsigned int getVerticesNum() { return num_vertices; };
	unsigned int getIndicesNum() { return num_indices; };
	math::AABB getBoundingBox() { return bounding_box; };

	void Save();
	u32 uuid;
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
	void LoadFile(std::string path);
	void Load();

	void LoadVBO();
	void LoadEBO();
	void CreateVAO();
};

class ComponentMesh : public Component
{
public:
	ComponentMesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices, ComponentMaterial* material, const char* name, const std::vector<float3>& obb_vertices, unsigned int id);
	ComponentMesh();
	~ComponentMesh();

	void Update(unsigned int program, const float3& position, const Quat& rotation, const float3& scale) override;
	void Draw(unsigned int program, const float3& position, const Quat& rotation, const float3& scale);

	void getBoundingBox(math::AABB& bbox) override;
	void getTriangles(std::vector<math::Triangle> &triangles) override;

	unsigned int GetVertices() { return mesh->getVerticesNum(); };
	unsigned int GetIndices() { return mesh->getIndicesNum(); };

	void printComponentInfo() override;
	void selectMaterial();

	void Save(Archive* archive) override;
	void LoadFile(std::string path) override;

	bool visible;

private:

	ComponentMaterial* comp_material;
	Mesh* mesh;
};