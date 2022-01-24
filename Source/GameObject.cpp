#include "GameObject.h"
#include "ComponentTransform.h"
#include "Model.h"
#include "imgui.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Scene.h"
#include "MathGeoLib.h"

GameObject::GameObject(unsigned int _id, GameObject* _parent)
{
	id = _id;
	name = "EmptyObject";
	parent = _parent;
	selected = false;
	active = true;
};

GameObject::~GameObject()
{
	for (Component* c : components)
		delete(c);
}

void GameObject::Load(const std::string &file_name, GoType _type)
{
	name = file_name;
	type = _type;
	components.clear();
	// All gameObjects have a transform
	ComponentTransform* transform = new ComponentTransform();
	components.push_back(transform);

	switch (type)
	{
	case GoType::Model:
	{
		Model* model = new Model();
		std::vector<GameObject*> t_vect = model->Load(file_name, this);
		addChildren(t_vect);
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
		console->AddLog("Creating empty object...");
		break;
	}
}

void GameObject::Update(unsigned int program)
{
	if (!active)
		return;

	float3 position, scale;
	Quat rotation;

	if (parent == nullptr)
	{
		position = { 0.0f, 0.0f, 0.0f };
		rotation = Quat::FromEulerXYZ(0.0f, 0.0f, 0.0f);
		scale = { 1.0f, 1.0f, 1.0f };
	}
	else 
	{
		position = parent->Transform()->getPos();
		rotation = parent->Transform()->getRot();
		scale = parent->Transform()->getSca();
	}

	for (GameObject* ch : children)
	{
		ch->Update(program);
	}

	for (Component* c : components)
	{
		c->Update(program, position, rotation, scale);
	}
}

ComponentTransform* GameObject::Transform()
{
	for (Component* c : components)
	{
		if (c->getType() == CompType::Transform)
		{
			ComponentTransform* aux_ptr = dynamic_cast<ComponentTransform*>(c);
			if (aux_ptr)
				return aux_ptr;
		}
	}
	return nullptr;
}


/*
	ImGui print for model info
*/
void GameObject::printGameObjectInfo()
{
	const ImVec4 title_colour(255, 255, 0, 255);
	ImGui::Checkbox("Active", &active); 
	ImGui::InputText("Name", &name[0], 64);

	ImGui::Separator();

	for (Component* c : components)
	{
		c->printComponentInfo();

		ImGui::Separator();
	}
}

void GameObject::printHierarchy(ImGuiTreeNodeFlags flags)
{
	ImGuiTreeNodeFlags node_flags = flags;
	if(selected)
		node_flags = ImGuiTreeNodeFlags_Selected;

	selected = false;

	if (children.size() > 0)
	{
		bool is_open = ImGui::TreeNodeEx((void*)this->id, node_flags, name.c_str());

		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
			App->scene->getScene(App->scene->current_scene)->selected_gameObject = this;

		if (is_open)
		{
			for (GameObject* go : children)
			{
				go->printHierarchy(flags);
			}

			ImGui::TreePop();
		}
	}
	else
	{
		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		ImGui::TreeNodeEx((void*)this->id, node_flags, "%s", name.c_str());

		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
			App->scene->getScene(App->scene->current_scene)->selected_gameObject = this;


	}
	
}