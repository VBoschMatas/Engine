#pragma once
#include "assimp/scene.h"
#include <vector>
#include <map>
#include "Globals.h"
#include "ModuleTexture.h"
#include "Mesh.h"
#include "Geometry/OBB.h"

struct Material
{
	unsigned int id;
	std::vector<unsigned int> texture_id;
};

class Model
{
public:
	Model() = default;
	~Model() = default;

	void Load(const std::string file_name);
	void Draw(unsigned int program);

	void PrintModelInfo();

	std::vector<Mesh> GetMeshes() { return meshes; };

	math::OBB bounding_box;
private:
	std::vector<Mesh> meshes;
	std::string directory;

	std::string model_name;

	// Vectors of materials and textures will be moved to the scene in a future iteration
	std::map<unsigned int, Texture> model_textures;
	std::vector<Material> model_materials;

	std::vector<Texture> LoadTextures(aiMaterial* material, unsigned int material_index, aiTextureType type);
	Mesh LoadMeshes(aiMesh* mesh, const aiScene* scene, std::vector<float3>& comb_vertices);
};

