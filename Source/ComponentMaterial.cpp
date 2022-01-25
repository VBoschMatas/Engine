#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Scene.h"

ComponentMaterial::ComponentMaterial(const aiMesh* mesh)
{
	if (mesh->mMaterialIndex >= 0)
		material = App->scene->getCurrentScene()->GetMaterials()[mesh->mMaterialIndex];
	else
		material = nullptr;
}

Material::Material(aiMaterial* material, const char* path, unsigned int id)
{
	id = App->scene->getCurrentScene()->getMaterialId();
	textures = LoadTextures(material, aiTextureType_DIFFUSE, path);
}

// Move Load Textures to Mesh
std::vector<Texture*> Material::LoadTextures(aiMaterial* material, aiTextureType type, const char* path)
{
	std::vector<Texture*> textures = {};

	std::vector<unsigned int> tex_hash_vect;
	unsigned int tex_hash;

	unsigned int n_textures = material->GetTextureCount(type);
	if (n_textures == 0)
	{
		bool temp;
		console->AddLog("This model has no textures!");
		textures.push_back( App->textures->LoadTexture("textures/default.jfif", temp));

		return textures;
	}

	for (unsigned int i = 0; i < n_textures; ++i)
	{
		aiString ai_path;
		material->GetTexture(type, i, &ai_path);
		bool texture_found = false;
		bool already_exists = false;
		// Searching in described path
		console->AddLog("		Checking for textures in described path");
		std::string mat_dir(path);
		mat_dir.append(ai_path.C_Str());
		Texture* texture;

		tex_hash = std::hash<std::string>{}(mat_dir);
		if (App->scene->getCurrentScene()->GetTextures().find(tex_hash) != App->scene->getCurrentScene()->GetTextures().end())
		{
			texture = App->scene->getCurrentScene()->GetTextures().find(tex_hash)->second;
			already_exists = true;
		}

		if (!already_exists)
			texture = App->textures->LoadTexture(mat_dir.c_str(), texture_found);


		std::string tex_name(ai_path.C_Str());
		// Searching in the same dir as the model
		if (!texture_found && !already_exists)
		{
			tex_name = tex_name.substr(0, tex_name.find_last_of('\\'));
			App->textures->UnloadTexture(1, &texture->id);
			std::string same_dir(path);
			same_dir.append(tex_name);
			console->AddLog("		Checking for textures in same directory as object: %s", same_dir.c_str());

			tex_hash = std::hash<std::string>{}(same_dir);
			if (App->scene->getCurrentScene()->GetTextures().find(tex_hash) != App->scene->getCurrentScene()->GetTextures().end())
			{
				texture = App->scene->getCurrentScene()->GetTextures().find(tex_hash)->second;
				already_exists = true;
			}

			if (!already_exists)
				texture = App->textures->LoadTexture(same_dir.c_str(), texture_found);
		}

		// Searching in the Game --> Textures dir
		if (!texture_found && !already_exists)
		{
			console->AddLog("		Checking for textures in textures directory");
			App->textures->UnloadTexture(1, &texture->id);
			std::string tex_dir;
			tex_dir = "textures\\";
			tex_dir.append(tex_name);

			tex_hash = std::hash<std::string>{}(tex_dir);
			if (App->scene->getCurrentScene()->GetTextures().find(tex_hash) != App->scene->getCurrentScene()->GetTextures().end())
			{
				texture = App->scene->getCurrentScene()->GetTextures().find(tex_hash)->second;
				already_exists = true;
			}
			if (!already_exists)
				texture = App->textures->LoadTexture(tex_dir.c_str(), texture_found);
		}

		// Texture not found
		if (!texture_found && !already_exists)
		{
			console->AddLog("		Textures not found!");
			App->textures->UnloadTexture(1, &texture->id);
			texture = App->textures->LoadTexture("textures/default.jfif", texture_found);
		}

		// We save the texture if it is new
		if (!already_exists)
		{
			App->scene->getCurrentScene()->AddTexture(tex_hash, texture);
		}
		else
			console->AddLog("	This texture was used previously");

		tex_hash_vect.push_back(tex_hash);
		textures.push_back(texture);
	}

	return textures;
}