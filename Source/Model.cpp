#include "Model.h"
#include <direct.h>
#include <shlwapi.h>
#include <algorithm>
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleScene.h"
#include "ComponentMaterial.h"
#include "ComponentTransform.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "assimp/material.h"
#include "assimp/Importer.hpp"
#include "GL/glew.h"
#include "IL/il.h"

std::vector<GameObject*> Model::Load(const std::string &file_name, GameObject* root)
{
	std::vector<GameObject*> children = {};

	Assimp::Importer import;

	size_t dir_length = std::string::npos;
	dir_length = file_name.find_last_of('/');
	if (dir_length == std::string::npos || dir_length == -1)
		dir_length = file_name.find_last_of('\\');

	if (dir_length != std::string::npos && dir_length != -1)
		model_name = file_name.substr(dir_length);
	else
		model_name = file_name;

	// We clean up the name
	std::string junk_chars = "/:*\?\"<>|\\";
	model_name.erase(std::remove_if(model_name.begin(), model_name.end(),
		[&junk_chars](const char& c) {
			return junk_chars.find(c) != std::string::npos;
		}),
		model_name.end());

	// We check if we had materials from previous objects, and set them as offset
	material_offset = App->scene->GetMaterials().size();

	console->AddLog("Reading object: %s", model_name.c_str());

	const aiScene* scene = import.ReadFile(file_name, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
	{
		console->AddLog("Error loading %s: %s", file_name, import.GetErrorString());
		return {};
	}

	// Linux absolutepath starts with '/' but for now we will focus on windows
	// Windows absolute path starts with a character for the hard drive and ':'
	if (file_name[1] == ':')
	{
		directory = file_name.substr(0, dir_length);
	}
	else
	{
		char buffer[MAX_PATH];
		_getcwd(buffer, MAX_PATH);
		if (dir_length != std::string::npos && dir_length != -1)
			directory = '\\' + file_name.substr(0, dir_length);
		directory = buffer + directory;
	}
	directory.append("\\");

	// Load all of the scene materials
	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
	{
		Material* material = new Material(scene->mMaterials[i]);
	}


	std::vector<float3> all_vertices;

	// Create all its children and components
	console->AddLog("Detected %d children", scene->mRootNode->mNumChildren);

	children = LoadChildren(scene->mRootNode, root, scene);

	std::vector<Component*> comp_list = {};
	comp_list = LoadComponents(scene->mRootNode, scene, root);

	root->addComponent(comp_list);

	root->setName(model_name.substr(0, std::string::size_type(model_name.find_last_of('.'))).c_str());

	console->AddLog("Setting initial object position");
	aiMatrix4x4 object_transform = scene->mRootNode->mTransformation;
	aiVector3D position, scale;
	aiQuaternion rotation;
	object_transform.Decompose(scale, rotation, position);
	root->Transform()->setPos(position.x, position.y, position.z);
	root->Transform()->setSca(scale.x, scale.y, scale.z);
	root->Transform()->setRot(rotation.x, rotation.y, rotation.z, rotation.w);

	return children;
}

std::vector<GameObject*> Model::LoadChildren(aiNode* aiParent, GameObject* goParent, const aiScene* scene)
{
	std::vector<GameObject*> children = {};
	for (unsigned int i = 0; i < aiParent->mNumChildren; ++i)
	{
		std::vector<Component*> comp_list = {};

		aiNode* new_go = aiParent->mChildren[i];
		// We create an empty object
		GameObject* new_child = App->scene->AddGameObject(new_go->mName.C_Str(), goParent, GoType::Empty);
		children.push_back(new_child);

		std::vector<GameObject*> grandchildren = {};
		grandchildren = LoadChildren(new_go, new_child, scene);

		comp_list = LoadComponents(new_go, scene, goParent);
		//App->scene->getScene(App->scene->current_scene)->AddGameObject()
		new_child->addComponent(comp_list);
		new_child->addChildren(grandchildren);

		aiMatrix4x4 object_transform = new_go->mTransformation;
		aiVector3D position, scale;
		aiQuaternion rotation;
		object_transform.Decompose(scale, rotation, position);
		new_child->Transform()->setPos(position.x, position.y, position.z);
		new_child->Transform()->setSca(scale.x, scale.y, scale.z);
		new_child->Transform()->setRot(rotation.x, rotation.y, rotation.z, rotation.w);
	}
	return children;
}

std::vector<Component*> Model::LoadComponents(aiNode* node, const aiScene* scene, GameObject* goParent)
{
	std::vector<Component*> comp_list = {};
	console->AddLog("Detected %d meshes", node->mNumMeshes);
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		unsigned int mesh_id = node->mMeshes[i];
		aiMesh* mesh = scene->mMeshes[mesh_id];

		console->AddLog("Loading mesh %s", scene->mMeshes[mesh_id]->mName.C_Str());
		std::vector<float3> all_vertices;
		std::pair<ComponentMesh*, ComponentMaterial*> aux_comp = LoadMeshes(mesh, scene, all_vertices, goParent);
		comp_list.push_back(aux_comp.first);
		comp_list.push_back(aux_comp.second);
	}
	return comp_list;
}

// We return the component mesh and the component material indexed to the mesh
std::pair<ComponentMesh*, ComponentMaterial*> Model::LoadMeshes(const aiMesh* mesh, const aiScene* scene, std::vector<float3>& comb_vertices, GameObject* goParent)
{
	console->AddLog("Loading mesh...");
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	console->AddLog("	Getting vertices");
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;
		float3 p_vector;

		p_vector.x = mesh->mVertices[i].x;
		p_vector.y = mesh->mVertices[i].y;
		p_vector.z = mesh->mVertices[i].z;
		vertex.position = p_vector;
		comb_vertices.push_back(p_vector);
		p_vector.x = mesh->mNormals[i].x;
		p_vector.y = mesh->mNormals[i].y;
		p_vector.z = mesh->mNormals[i].z;
		vertex.normal = p_vector;

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
	console->AddLog("	Getting indices");
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	ComponentMaterial* material = new ComponentMaterial(mesh, material_offset, goParent->getCompId());
	ComponentMesh* c_mesh = new ComponentMesh(vertices, indices, material, mesh->mName.C_Str(), comb_vertices, goParent->getCompId());

	return std::pair<ComponentMesh*, ComponentMaterial*>(c_mesh ,material);
}