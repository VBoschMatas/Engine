#pragma once
#include "assimp/scene.h"
#include <vector>
#include <map>
#include "Globals.h"
#include "ModuleTexture.h"
#include "ComponentMesh.h"
#include "Geometry/OBB.h"
#include "Component.h"
#include "GameObject.h"
#include "Scene.h"

class Model
{
public:
	Model() = default;
	~Model() = default;

	std::vector<GameObject*> Load(const std::string &file_name, GameObject* root);

	void PrintModelInfo();

	float3 GetPosition() { return position; };
	float3 GetRotation() { return rotation; };

	std::vector<ComponentMesh*> GetMeshes() { return meshes; };

	math::OBB bounding_box;
private:
	float3 position = {0.0f, 0.0f, 0.0f};
	float3 rotation = {0.0f, 0.0f, 0.0f};

	std::vector<ComponentMesh*> meshes;
	std::string directory;

	std::string model_name;

	// Vectors of materials and textures will be moved to the scene in a future iteration
	std::map<unsigned int, Texture> model_textures;
	std::vector<Material> model_materials;

	std::vector<Texture> LoadTextures(aiMaterial* material, unsigned int material_index, aiTextureType type);
	ComponentMesh* LoadMeshes(const aiMesh* mesh, const aiScene* scene, std::vector<float3>& comb_vertices);
	std::vector<GameObject*> LoadChildren(aiNode* aiParent, GameObject* goParent,const aiScene* scene);
};

