#pragma once
#include "assimp/scene.h"
#include <vector>
#include <map>
#include "Globals.h"
#include "ModuleTexture.h"
#include "Mesh.h"
#include "Geometry/OBB.h"
#include "Component.h"
#include "Scene.h"

class Model
{
public:
	Model() = default;
	~Model() = default;

	std::vector<Component*> Load(const std::string &file_name);

	void PrintModelInfo();

	void SetPosition(float x, float y, float z)
	{
		position = { x, y, z };
	};

	void SetRotation(float x, float y, float z)
	{
		rotation = { x, y, z };
	};

	float3 GetPosition() { return position; };
	float3 GetRotation() { return rotation; };

	std::vector<Mesh*> GetMeshes() { return meshes; };

	math::OBB bounding_box;
private:
	float3 position = {0.0f, 0.0f, 0.0f};
	float3 rotation = {0.0f, 0.0f, 0.0f};

	std::vector<Mesh*> meshes;
	std::string directory;

	std::string model_name;

	// Vectors of materials and textures will be moved to the scene in a future iteration
	std::map<unsigned int, Texture> model_textures;
	std::vector<Material> model_materials;

	std::vector<Texture> LoadTextures(aiMaterial* material, unsigned int material_index, aiTextureType type);
	Mesh* LoadMeshes(const aiMesh* mesh, const aiScene* scene, std::vector<float3>& comb_vertices);
};

