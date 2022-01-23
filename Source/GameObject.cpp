#include "GameObject.h"
#include "Model.h"
#include "imgui.h"

GameObject::GameObject(unsigned int _id)
{
	id = _id;
	name = "EmptyObject";
	parent = nullptr;
};

void GameObject::Load(const std::string &file_name, GoType _type)
{
	name = file_name;
	type = _type;

	switch (type)
	{
	case GoType::Model:
	{
		Model* model = new Model();
		std::vector<Component*> t_vect = model->Load(file_name);
		components.insert(components.end(), t_vect.begin(), t_vect.end());
		delete(model);
	}
	break;
	case GoType::Camera:
		name = "Camera";
		break;
	case GoType::Text:
		name = "Text";
		break;
	default:
		break;
	}
}

void GameObject::Update(unsigned int program)
{
	float3 position{0.0f, 0.0f, 0.0f}, rotation{ 0.0f, 0.0f, 0.0f }, scale{ 0.0f, 0.0f, 0.0f };

	for (Component* c : components)
	{
		c->Update(program, position, rotation, scale);
	}
}


/*
	ImGui print for model info
*/
void GameObject::PrintGameObjectInfo()
{
	const ImVec4 title_colour(255, 255, 0, 255);

	ImGui::TextColored(title_colour, "Model");
	ImGui::Separator();
	ImGui::TextWrapped("Name: %s", name.c_str());
	ImGui::TextWrapped("Type: %s", type);
	ImGui::Separator();
	ImGui::TextColored(title_colour, "Textures");
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