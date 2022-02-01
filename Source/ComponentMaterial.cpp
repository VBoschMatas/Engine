#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleScene.h"
#include <map>

ComponentMaterial::ComponentMaterial(const aiMesh* mesh, unsigned int offset, unsigned int id)
{
	this->id = id;
	if (mesh != nullptr && mesh->mMaterialIndex >= 0)
		material = App->scene->GetMaterials()[((size_t)mesh->mMaterialIndex + (size_t)offset)];
	else
		material = nullptr;
}

Material::Material()
{
	id = App->scene->getMaterialId();
	name = "NewMaterial" + std::to_string(id);
	textures[1] = nullptr;

	ambient = float3(1.0f, 1.0f, 1.0f);
	diffuse = float3(1.0f, 1.0f, 1.0f);
	specular = float3(1.0f, 1.0f, 1.0f);

	metallic = 0.0f;
	albedo = 1.0f;
	smoothness = 0.4f;
	bool temp;
	default_texture = App->textures->LoadTexture("textures/default.png", temp);
	textures[0] = &default_texture;
}

Material::Material(aiMaterial* material, const char* path)
{
	id = App->scene->getMaterialId();
	name = "NewMaterial" + std::to_string(id);
	textures[0] = LoadTextures(material, aiTextureType_DIFFUSE, path);
	textures[1] = LoadTextures(material, aiTextureType_SPECULAR, path);
	//textures[2] = LoadTextures(material, aiTextureType_SPECULAR, path);
	//textures[3] = LoadTextures(material, aiTextureType_LIGHTMAP, path);

	ambient = float3(1.0f, 1.0f, 1.0f);
	diffuse = float3(1.0f, 1.0f, 1.0f);
	specular = float3(1.0f, 1.0f, 1.0f);

	metallic = 0.0f;
	albedo = 1.0f;
	smoothness = 0.4f;
	bool temp;
	default_texture = App->textures->LoadTexture("textures/default.png", temp);

	if (textures[0] == nullptr)
	{
		textures[0] = &default_texture;
	}
}

// Move Load Textures to Mesh
Texture* Material::LoadTextures(aiMaterial* material, aiTextureType type, const char* path)
{
	unsigned int tex_hash;

	unsigned int n_textures = material->GetTextureCount(type);
	if (n_textures == 0)
	{
		//bool temp;
		console->AddLog("This model has no textures!");
		return {};
	}

	aiString ai_path;
	material->GetTexture(type, 0, &ai_path);
	bool texture_found = false;
	bool already_exists = false;
	// Searching in described path
	console->AddLog("		Checking for textures in described path");
	std::string mat_dir(path);
	mat_dir.append(ai_path.C_Str());
	Texture* texture = nullptr;
	Texture temp_texture;

	tex_hash = std::hash<std::string>{}(mat_dir);

	std::map<unsigned int, Texture>::iterator it = App->scene->GetTextures()->find(tex_hash);
	std::map<unsigned int, Texture>::iterator end_it = App->scene->GetTextures()->end();
	if (it != end_it)
	{
		texture = &it->second;
		already_exists = true;
	}

	if (!already_exists)
		temp_texture = App->textures->LoadTexture(mat_dir.c_str(), texture_found);


	std::string tex_name(ai_path.C_Str());
	// Searching in the same dir as the model
	if (!texture_found && !already_exists)
	{
		tex_name = tex_name.substr(0, tex_name.find_last_of('\\'));
		App->textures->UnloadTexture(1, &temp_texture.id);
		std::string same_dir(path);
		same_dir.append(tex_name);
		console->AddLog("		Checking for textures in same directory as object: %s", same_dir.c_str());

		tex_hash = std::hash<std::string>{}(same_dir);
		std::map<unsigned int, Texture>::iterator it = App->scene->GetTextures()->find(tex_hash);
		std::map<unsigned int, Texture>::iterator end_it = App->scene->GetTextures()->end();
		if (it != end_it)
		{
			texture = &App->scene->GetTextures()->find(tex_hash)->second;
			already_exists = true;
		}

		if (!already_exists)
			temp_texture = App->textures->LoadTexture(same_dir.c_str(), texture_found);
	}

	// Searching in the Game --> Textures dir
	if (!texture_found && !already_exists)
	{
		console->AddLog("		Checking for textures in textures directory");
		App->textures->UnloadTexture(1, &temp_texture.id);
		std::string tex_dir;
		tex_dir = "textures\\";
		tex_dir.append(tex_name);

		tex_hash = std::hash<std::string>{}(tex_dir);
		std::map<unsigned int, Texture>::iterator it = App->scene->GetTextures()->find(tex_hash);
		std::map<unsigned int, Texture>::iterator end_it = App->scene->GetTextures()->end();
		if (it != end_it)
		{
			texture = &App->scene->GetTextures()->find(tex_hash)->second;
			already_exists = true;
		}
		if (!already_exists)
			temp_texture = App->textures->LoadTexture(tex_dir.c_str(), texture_found);
	}

	// Texture not found
	if (!texture_found && !already_exists)
	{
		console->AddLog("		Textures not found!");
		App->textures->UnloadTexture(1, &temp_texture.id);
		return {};
		//texture = App->textures->LoadTexture("textures/default.png", texture_found);
	}

	// We save the texture if it is new
	if (!already_exists)
	{
		texture = App->scene->AddTexture(tex_hash, temp_texture);
	}
	else
		console->AddLog("	This texture was used previously");

	return texture;
}

void Material::addTexture(const char* path, unsigned int tex_id)
{
	if(textures[tex_id] != nullptr)
		App->textures->UnloadTexture(1, &textures[tex_id]->id);

	bool temp;
	Texture temp_texture = App->textures->LoadTexture(path, temp);
	unsigned int tex_hash = std::hash<std::string>{}(path);
	textures[tex_id] = App->scene->AddTexture(tex_hash, temp_texture);
}

void Material::RemoveTexture(unsigned int tex_id)
{
	if (tex_id == 0)
	{
		if(textures[tex_id] != &default_texture)
			textures[tex_id] = &default_texture;
	}
	else
	{
		textures[tex_id] = nullptr;
	}
}

void Material::setTexture(Texture* tex, unsigned int tex_id)
{
	textures[tex_id] = tex;
}

void ComponentMaterial::printComponentInfo()
{
	const ImVec4 yellow_colour(255, 255, 0, 255);
	static ImGuiTreeNodeFlags texture_flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
	static ImGuiTreeNodeFlags header_flags = ImGuiTreeNodeFlags_DefaultOpen;

	std::string itemid = "Material ##" + std::to_string(this->id);
	if (ImGui::CollapsingHeader(itemid.c_str(), header_flags))
	{
		ImGui::InputText("Name", &material->name[0], 64);
		ImGui::Text("");
		ImGui::DragFloat3("Ambient", (float*)&material->ambient, 0.005f, 0.0f, +FLT_MAX, "%.3f", 1.0f);
		ImGui::DragFloat3("Diffuse", (float*)&material->diffuse, 0.005f, 0.0f, +FLT_MAX, "%.3f", 1.0f);
		ImGui::DragFloat3("Specular", (float*)&material->specular, 0.005f, 0.0f, +FLT_MAX, "%.3f", 1.0f);
		ImGui::Text("");
		ImGui::SliderFloat("Albedo", &material->albedo, 0.0f, 1.0, "%.3f", 1.0f);
		ImGui::SliderFloat("Metallic", &material->metallic, 0.0f, 1.0, "%.3f", 1.0f);
		ImGui::SliderFloat("Smoothness", &material->smoothness, 0.0f, 1.0, "%.3f", 1.0f);
		ImGui::Text("");
		for (unsigned int i = 0; i < 2; ++i)
		{
			std::string sel_name = "##" + std::to_string(this->id) + "," + std::to_string(i);
			if (ImGui::TreeNodeEx((void*)(this + i), texture_flags, material->getTexTypes()[i]))
			{
				showImage(i);
				if (ImGui::Button(std::string("Select Texture" + sel_name).c_str()))
				{
					if (!ImGui::IsPopupOpen(std::string("Select Texture" + sel_name).c_str()))
						ImGui::OpenPopup(std::string("Select Texture" + sel_name).c_str());
					ImVec2 center = ImGui::GetMainViewport()->GetCenter();
					ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
				}
				selectTexture(i);
				if (ImGui::Button(std::string("Delete Texture" + sel_name).c_str()))
				{
					material->RemoveTexture(i);
				}
				ImGui::EndGroup();
				ImGui::TextWrapped("\n");
				ImGui::TreePop();
			}
		}
	}
}

void ComponentMaterial::showImage(unsigned int i)
{
	const ImVec4 yellow_colour(255, 255, 0, 255);
	if (material->getTextures()[i] == nullptr)
	{
		ImGui::Image(nullptr, ImVec2(80, 80), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
		ImGui::SameLine();
		ImGui::BeginGroup();
		ImGui::Text("");
		ImGui::TextWrapped("Path: ");
	}
	else
	{
		ImGui::Image((void*)(intptr_t)material->getTextures()[i]->id, ImVec2(80, 80), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
		ImGui::SameLine();
		ImGui::BeginGroup();
		ImGui::TextColored(yellow_colour, material->getTextures()[i]->name.c_str());
		ImGui::SameLine();
		ImGui::Text("%dx%d", material->getTextures()[i]->width, material->getTextures()[i]->height);
		ImGui::TextWrapped("Path: %s", material->getTextures()[i]->path.c_str());
	}
}

void ComponentMaterial::selectTexture(unsigned int tex_id)
{
	std::string sel_name = "Select Texture##" + std::to_string(this->id) + "," + std::to_string(tex_id);
	if (ImGui::BeginPopupModal(sel_name.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Select one of the scene's textures.");
		ImGui::Text("You can add new textures by dropping them.");
		if (ImGui::BeginChildFrame(ImGui::GetID("frame"), ImVec2(-FLT_MIN, 250)))
		{
			std::map<unsigned int, Texture>* temp_textures = App->scene->GetTextures();
			for (auto & t : *temp_textures)
			{
				std::string itemid = "##texsel" + std::to_string(t.second.id);
				if (ImGui::Selectable(itemid.c_str(), false, 0, ImVec2(0, 40))) {
					material->setTexture(&t.second, tex_id);
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				ImGui::Image((void*)(intptr_t)t.second.id, ImVec2(40, 40));
				ImGui::SameLine();
				ImGui::Text(t.second.name.c_str());
			}
			ImGui::EndChildFrame();
		}

		ImVec2 button_size(ImGui::GetFontSize() * 7.0f, 0.0f);
		if (ImGui::Button("Cancel", button_size))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}