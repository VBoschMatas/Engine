#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleScene.h"
#include "MaterialImporter.h"
#include "MathGeoLib.h"
#include <map>

ComponentMaterial::ComponentMaterial(const aiMesh* mesh, unsigned int offset, unsigned int id)
{
	math::LCG math;
	uuid = math.Int();
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
	App->scene->AddMaterial(this);
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

Material::Material(aiMaterial* material)
{
	math::LCG math;
	uuid = math.Int();
	id = App->scene->getMaterialId();
	App->scene->AddMaterial(this);
	MaterialImporter::Import(material, this);
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


Texture* Material::LoadTextures(const char* path)
{
	unsigned int tex_hash;

	bool texture_found = false;
	bool already_exists = false;
	// Searching in described path
	console->AddLog("		Checking for texture in described path");
	std::string mat_dir(path);

	console->AddLog(mat_dir.c_str());
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


	std::string tex_name(path);
	// Searching in the Game --> Textures dir
	if (!texture_found && !already_exists)
	{
		console->AddLog("		Checking for texture in textures directory");
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
		console->AddLog("		Texture not found!");
		App->textures->UnloadTexture(1, &temp_texture.id);
		return nullptr;
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

	textures[tex_id] = LoadTextures(path);
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

void Material::Save()
{
	Archive* go_archive = new Archive();
	go_archive->json = {
		{"Name", "Material"},
		{"UUID", uuid},
		{"Ambient", {ambient.x, ambient.y, ambient.z}},
		{"Diffuse", {diffuse.x, diffuse.y, diffuse.z}},
		{"Specular", {specular.x, specular.y, specular.z}},
		{"Metallic", metallic},
		{"Albedo", albedo},
		{"Smoothness", smoothness}
	};
	if (textures[0] != nullptr)
		go_archive->json["TexDiffuse"] = textures[0]->name.c_str();
	else go_archive->json["TexDiffuse"] = "";
	if (textures[1] != nullptr)
		go_archive->json["TexSpecular"] = textures[0]->name.c_str();
	else go_archive->json["TexSpecular"] = "";
	go_archive->ToFile();
}

void ComponentMaterial::Save(Archive* archive)
{
	archive->json["Component"] += {uuid, (int)type};
	Archive* go_archive = new Archive();
	go_archive->json = {
		{"Name", "Material"},
		{"UUID", uuid},
		{"Material", material->uuid}
	};
	material->Save();
	go_archive->ToFile();
}