#include "Model.h"
#include "Application.h"
#include "Globals.h"
#include "ModuleTexture.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "assimp/material.h"

void Model::Load(const char* file_name)
{
	const aiScene* scene = aiImportFile(file_name, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene)
	{
		// TODO: LoadTetures(scene->Maerials, scene->mNumMaterials);
		// TODO: LoadMeshes(scene->mMeshes, scene->mNumMeshes);
	}
	else
	{
		DEBUG("Error loading %s: %s", file_name, aiGetErrorString);
	}
}

void Model::LoadMaterials(const aiScene* scene)
{
	aiString file;
	materials.reserve(scene->mNumMaterials);
	for (unsigned i = 0; i < scene->mNumMaterials; ++i)
	{
		if (scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &file) == AI_SUCCESS)
		{
			materials.push_back(App->textures->LoadTexture(file.data));
		}
	}
}