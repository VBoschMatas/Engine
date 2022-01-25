#include "Model.h"
#include <direct.h>
#include <shlwapi.h>
#include <algorithm>
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleScene.h"
#include "ComponentMaterial.h"
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
		Material* material = new Material(scene->mMaterials[i], directory.c_str(), i);
		App->scene->getScene(App->scene->current_scene)->AddMaterial(material);
	}


	std::vector<float3> all_vertices;

	// Create all its children and components
	console->AddLog("Detected %d children", scene->mRootNode->mNumChildren);

	children = LoadChildren(scene->mRootNode, root, scene);

	std::vector<Component*> comp_list = {};
	comp_list = LoadComponents(scene->mRootNode, scene);

	root->addComponent(comp_list);

	console->AddLog("Creating OBB for the object");
	//bounding_box = OBB::OptimalEnclosingOBB(&all_vertices[0], all_vertices.size());

	root->setName(model_name.substr(0, std::string::size_type(model_name.find_last_of('.'))).c_str());
	//scene->mRootNode->mTransformation   TO DEVELOP

	return children;
}

std::vector<GameObject*> Model::LoadChildren(aiNode* aiParent, GameObject* goParent, const aiScene* scene)
{
	std::vector<GameObject*> children = {};
	for (unsigned int i = 0; i < aiParent->mNumChildren; ++i)
	{
		std::vector<Component*> comp_list = {};

		aiNode* new_go = aiParent->mChildren[i];
		GameObject* new_child = App->scene->getScene(App->scene->current_scene)->AddGameObject(new_go->mName.C_Str(), goParent, GoType::Empty);
		children.push_back(new_child);

		std::vector<GameObject*> grandchildren = {};
		grandchildren = LoadChildren(new_go, new_child, scene);

		comp_list = LoadComponents(new_go, scene);
		//App->scene->getScene(App->scene->current_scene)->AddGameObject()
		new_child->addComponent(comp_list);
		new_child->addChildren(grandchildren);

		//std::vector<Component*> compo = children.back()->getComponents();
	}
	return children;
}

std::vector<Component*> Model::LoadComponents(aiNode* node, const aiScene* scene)
{
	std::vector<Component*> comp_list = {};
	console->AddLog("Detected %d meshes", node->mNumMeshes);
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		unsigned int mesh_id = node->mMeshes[i];
		aiMesh* mesh = scene->mMeshes[mesh_id];

		console->AddLog("Loading mesh %s", scene->mMeshes[mesh_id]->mName.C_Str());
		std::vector<float3> all_vertices;
		std::pair<ComponentMesh*, ComponentMaterial*> aux_comp = LoadMeshes(mesh, scene, all_vertices);
		comp_list.push_back(aux_comp.first);
		comp_list.push_back(aux_comp.second);
	}
	return comp_list;
}

// We return the component mesh and the component material indexed to the mesh
std::pair<ComponentMesh*, ComponentMaterial*> Model::LoadMeshes(const aiMesh* mesh, const aiScene* scene, std::vector<float3>& comb_vertices)
{
	console->AddLog("Loading mesh...");
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
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

	// To Component material

	/*bool existing_material = false;
	for (auto const& m : model_materials)
	{
		if (m.id == mesh->mMaterialIndex)
		{
			for (auto const& t : m.texture_id)
			{
				textures.push_back(model_textures.find(t)->second);
			}
			existing_material = true;
			console->AddLog("	Material already loaded");
			break;
		}
	}

	if (mesh->mMaterialIndex >= 0 && !existing_material)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> tex = LoadTextures(material, mesh->mMaterialIndex, aiTextureType_DIFFUSE);
		textures.insert(textures.end(), tex.begin(), tex.end());
	}*/

	ComponentMaterial* material = new ComponentMaterial(mesh);

	return std::pair<ComponentMesh*, ComponentMaterial*>(new ComponentMesh(vertices, indices, textures), new ComponentMaterial(mesh));
}

std::vector<ComponentMaterial*> Model::LoadMaterials(const aiScene* scene)
{

}

// Move Load Textures to MATERIAL
std::vector<Texture> Model::LoadTextures(aiMaterial* material, unsigned int material_index, aiTextureType type)
{
	std::vector<Texture> textures = {};

	//std::vector<unsigned int> tex_hash_vect;
	//unsigned int tex_hash;

	//unsigned int n_textures = material->GetTextureCount(type);
	//if (n_textures == 0)
	//{
	//	bool temp;
	//	console->AddLog("This model has no textures!");
	//	textures.push_back(App->textures->LoadTexture("textures/default.jfif", temp));

	//	return textures;
	//}

	//for (unsigned int i = 0; i < n_textures; ++i)
	//{
	//	aiString path;
	//	material->GetTexture(type, i, &path);
	//	bool texture_found = false;
	//	bool already_exists = false;
	//	// Searching in described path
	//	console->AddLog("		Checking for textures in described path");
	//	std::string mat_dir(directory);
	//	mat_dir.append(path.C_Str());
	//	Texture texture;

	//	tex_hash = std::hash<std::string>{}(mat_dir);
	//	if (model_textures.find(tex_hash) != model_textures.end())
	//	{
	//		texture = model_textures.find(tex_hash)->second;
	//		already_exists = true;
	//	}

	//	if (!already_exists)
	//		texture = App->textures->LoadTexture(mat_dir.c_str(), texture_found);


	//	std::string tex_name(path.C_Str());
	//	// Searching in the same dir as the model
	//	if (!texture_found && !already_exists)
	//	{
	//		tex_name = tex_name.substr(0, tex_name.find_last_of('\\'));
	//		App->textures->UnloadTexture(1, &texture.id);
	//		std::string same_dir(directory);
	//		same_dir.append(tex_name);
	//		console->AddLog("		Checking for textures in same directory as object: %s", same_dir.c_str());

	//		tex_hash = std::hash<std::string>{}(same_dir);
	//		if (model_textures.find(tex_hash) != model_textures.end())
	//		{
	//			texture = model_textures.find(tex_hash)->second;
	//			already_exists = true;
	//		}

	//		if (!already_exists)
	//			texture = App->textures->LoadTexture(same_dir.c_str(), texture_found);
	//	}

	//	// Searching in the Game --> Textures dir
	//	if (!texture_found && !already_exists)
	//	{
	//		console->AddLog("		Checking for textures in textures directory");
	//		App->textures->UnloadTexture(1, &texture.id);
	//		std::string tex_dir;
	//		tex_dir = "textures\\";
	//		tex_dir.append(tex_name);

	//		tex_hash = std::hash<std::string>{}(tex_dir);
	//		if (model_textures.find(tex_hash) != model_textures.end())
	//		{
	//			texture = model_textures.find(tex_hash)->second;
	//			already_exists = true;
	//		}
	//		if (!already_exists)
	//			texture = App->textures->LoadTexture(tex_dir.c_str(), texture_found);
	//	}

	//	// Texture not found
	//	if (!texture_found && !already_exists)
	//	{
	//		console->AddLog("		Textures not found!");
	//		App->textures->UnloadTexture(1, &texture.id);
	//		texture = App->textures->LoadTexture("textures/default.jfif", texture_found);
	//	}

	//	// We save the texture if it is new
	//	if (!already_exists)
	//	{
	//		model_textures.insert(std::pair<unsigned int, Texture>(tex_hash, texture));
	//	}
	//	else
	//		console->AddLog("	This texture was used previously");

	//	tex_hash_vect.push_back(tex_hash);
	//	textures.push_back(texture);
	//}

	//model_materials.push_back(Material{ material_index, tex_hash_vect });

	return textures;
}

/*
	ImGui print for model info
*/
void Model::PrintModelInfo()
{
	const ImVec4 title_colour(255, 255, 0, 255);

	ImGui::TextColored(title_colour, "Model");
	ImGui::Separator();
	ImGui::TextWrapped("Name: %s", model_name.substr(0, std::string::size_type(model_name.find_last_of('.'))).c_str());
	ImGui::TextWrapped("Type: %s", model_name.substr(std::string::size_type(model_name.find_last_of('.'))).c_str());

	ImGui::TextWrapped("\n");
	ImGui::TextColored(title_colour, "Textures");
	ImGui::Separator();
	for (auto const& t : model_textures)
	{
		ImGui::TextWrapped("Path: %s", t.second.path.c_str());
		ImGui::Image((void*)(intptr_t)t.second.id, ImVec2(200, 200));
		ImGui::TextWrapped("%dx%d", t.second.width, t.second.height);
		ImGui::TextWrapped("\n");
	}
	ImGui::TextColored(title_colour, "Meshes");
	ImGui::Separator();
	ImGui::TextWrapped("Num of meshes: %d", meshes.size());
	unsigned int total_t = 0, total_v = 0, total_i = 0;
	for (int i = 0; i < meshes.size(); ++i)
	{
		ImGui::TextWrapped("[%d]T: %d  V: %d  I: %d", i, meshes[i]->GetIndices() / 3, meshes[i]->GetVertices(), meshes[i]->GetIndices());
		total_t += meshes[i]->GetIndices() / 3;
		total_v += meshes[i]->GetVertices();
		total_i += meshes[i]->GetIndices();
	}
	ImGui::TextWrapped("Total: T: %d  V: %d  I: %d", total_t, total_v, total_i);
}