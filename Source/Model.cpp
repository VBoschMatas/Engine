#include "Model.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleTexture.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "assimp/material.h"
#include "assimp/Importer.hpp"
#include "GL/glew.h"
#include "IL/il.h"

void Model::Load(std::string file_name)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(file_name, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
	{
		DEBUG("Error loading %s: %s", file_name, import.GetErrorString());
		return;
	}

	directory = file_name.substr(0, file_name.find_last_of('/'));

	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[i];
		meshes.push_back(LoadMeshes(mesh, scene));
	}
	/*const aiScene* scene = aiImportFile(file_name, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene)
	{
		LoadTextures(scene->mMaterials, scene->mNumMaterials);
		LoadMeshes(scene->mMeshes, scene->mNumMeshes);
	}
	else
	{
		DEBUG("Error loading %s: %s", file_name, aiGetErrorString);
	}*/
}

//void Model::LoadMaterials(const aiScene* scene)
//{
//	aiString file;
//	materials.reserve(scene->mNumMaterials);
//	for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
//	{
//		if (scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &file) == AI_SUCCESS)
//		{
//			materials.push_back(App->textures->LoadTexture(file.data));
//		}
//	}
//}
//
//void Model::LoadTextures(aiMaterial** material_array, unsigned int num_materials)
//{
//	unsigned int program = App->program->program;
//
//	for (unsigned int i = 0; i < num_materials; ++i)
//	{
//		glGenTextures(1, &texture_id);
//		glBindTexture(GL_TEXTURE_2D, texture_id);
//
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//
//		unsigned int img_id = App->textures->LoadTexture("textures/Lenna.png");
//
//		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH),
//			ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
//			ilGetData());
//		glGenerateMipmap(GL_TEXTURE_2D);
//
//		glUseProgram(program);
//		glUniform1i(glGetUniformLocation(program, "mytexture"), 0);
//
//		ilDeleteImages(1, &img_id);
//	}
//}

Mesh Model::LoadMeshes(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<unsigned int> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;
		float3 p_vector;

		p_vector.x = mesh->mVertices[i].x;
		p_vector.y = mesh->mVertices[i].y;
		p_vector.z = mesh->mVertices[i].z;
		vertex.position = p_vector;

		if (mesh->mTextureCoords[0])
		{
			float2 t_vector;
			t_vector.x = mesh->mTextureCoords[0][i].x;
			t_vector.y = mesh->mTextureCoords[0][i].y;
			vertex.uv = t_vector;
		}
		else
			vertex.uv = float2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<unsigned int> tex = LoadTextures(material, aiTextureType_DIFFUSE);
		textures.insert(textures.end(), tex.begin(), tex.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<unsigned int> Model::LoadTextures(aiMaterial* material, aiTextureType type)
{
	std::vector<unsigned int> textures;

	for (unsigned int i = 0; i < material->GetTextureCount(type); ++i)
	{
		aiString path;
		material->GetTexture(type, i, &path);
		unsigned int texture = App->textures->LoadTexture(path.C_Str());
		textures.push_back(texture);
		App->textures->UnloadTexture(1, &texture);

		DEBUG("Texture %d: %d", i, textures[i]);
	}
	
	return textures;
}

void Model::Draw(unsigned int program)
{
	for (unsigned int i = 0; i < meshes.size(); ++i)
	{
		meshes[i].Draw(program);
	}
}