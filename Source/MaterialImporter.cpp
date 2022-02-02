#include "Application.h"
#include "ModuleScene.h"
#include "MaterialImporter.h"
#include "ComponentMaterial.h"
#include "assimp/material.h"
#include "DevIL/include/IL/il.h"

void MaterialImporter::Import(const aiMaterial* material, Material* ourObject)
{
	aiString name; material->Get(AI_MATKEY_NAME, name);
	ourObject->name = name.C_Str();
	aiString texture_path;
	if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0)
	{
		material->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path);
		ourObject->addTexture(texture_path.C_Str(), 0);
	}
	if (material->GetTextureCount(aiTextureType_SPECULAR) != 0)
	{
		material->GetTexture(aiTextureType_SPECULAR, 0, &texture_path);
		ourObject->addTexture(texture_path.C_Str(), 1);
	}
	aiColor3D diff, spec, amb;
	material->Get(AI_MATKEY_COLOR_DIFFUSE, diff);
	material->Get(AI_MATKEY_COLOR_SPECULAR, spec);
	material->Get(AI_MATKEY_COLOR_AMBIENT, amb);
	ourObject->ambient = float3(amb.r, amb.g, amb.b);
	ourObject->diffuse = float3(diff.r, diff.g, diff.b);
	ourObject->specular = float3(spec.r, spec.g, spec.b);
}

std::uint64_t MaterialImporter::Save(const Material* mat, char** fileBuffer)
{
	
	return 0;
}

void MaterialImporter::Load(const char* fileBuffer, Material* ourObject)
{

}