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

	math::OBB bounding_box;
private:
	unsigned int material_offset;

	std::vector<ComponentMesh*> meshes;
	std::string directory;

	std::string model_name;

	std::pair<ComponentMesh*, ComponentMaterial*> LoadMeshes(const aiMesh* mesh, const aiScene* scene, std::vector<float3>& comb_vertices);
	std::vector<GameObject*> LoadChildren(aiNode* aiParent, GameObject* goParent, const aiScene* scene);
	std::vector<Component*> LoadComponents(aiNode* node, const aiScene* scene);
};

