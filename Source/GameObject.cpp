#include "GameObject.h"
#include "ComponentTransform.h"
#include "Model.h"
#include "imgui.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "ModuleEditorCamera.h"
#include "Scene.h"
#include "MathGeoLib.h"
#include "Math/float4x4.h"
#include "debugdraw.h"

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
	parent->removeChild(this);

	for (GameObject* ch : children)
		delete(ch);
	for (Component* c : components)
		delete(c);
}

void GameObject::Load(const std::string &file_name, GoType _type)
{
	name = file_name;
	type = _type;
	local_bbox.SetNegativeInfinity();
	world_bbox.SetNegativeInfinity();
	components.clear();
	children.clear();
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
		position = parent->Transform()->getWorldPos();
		rotation = parent->Transform()->getWorldRot();
		scale = parent->Transform()->getWorldSca();
	}

	for (Component* c : components)
	{
		c->Update(program, position, rotation, scale);
		c->getBoundingBox(local_bbox);
	}

	for (GameObject* ch : children)
	{
		ch->Update(program);
	}

	// Bounding box Transform
	if (local_bbox.IsFinite())
	{
		world_bbox = local_bbox;
		world_bbox.Transform(float4x4::FromTRS(position, rotation, scale));
	}

	selected = App->scene->getCurrentScene()->selected_gameObject == this;

	if (active && (selected || App->editor->isDebugDraw()))
		DebugDraw();
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
	const ImVec4 yellow_colour(255, 255, 0, 255);
	ImGui::Checkbox("Active", &active); 
	ImGui::InputText("Name", &name[0], 64);

	for (Component* c : components)
	{
		c->printComponentInfo();
	}
}

void GameObject::printHierarchy(ImGuiTreeNodeFlags flags)
{
	ImGuiTreeNodeFlags node_flags = flags;
	if(selected)
		node_flags = ImGuiTreeNodeFlags_Selected;

	if (children.size() > 0)
	{
		bool is_open = ImGui::TreeNodeEx((void*)this, node_flags, name.c_str());

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsItemToggledOpen())
			App->scene->getCurrentScene()->selected_gameObject = this;

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
		ImGui::TreeNodeEx((void*)this, node_flags, "%s", name.c_str());

		if ((ImGui::IsItemClicked(ImGuiMouseButton_Left) || ImGui::IsItemClicked(ImGuiMouseButton_Right)) && !ImGui::IsItemToggledOpen())
			App->scene->getCurrentScene()->selected_gameObject = this;


	}
}

void GameObject::DebugDraw()
{

	for (GameObject* ch : children)
	{
		ch->DebugDraw();
	}

	for (Component* c : components)
	{
		float3 extreme_points[8];
		world_bbox.GetCornerPoints(&extreme_points[0]);
		std::swap(extreme_points[2], extreme_points[5]);
		std::swap(extreme_points[3], extreme_points[4]);
		std::swap(extreme_points[4], extreme_points[5]);
		std::swap(extreme_points[6], extreme_points[7]);
		dd::box(extreme_points, dd::colors::White);
	}

	for (Component* c : components)
	{
		c->DebugDraw();
	}

}

bool GameObject::operator > (const GameObject*& str) const
{
	return world_bbox.Distance(App->editorcamera->GetPosition()) > str->world_bbox.Distance(App->editorcamera->GetPosition());
}

bool GameObject::operator < (const GameObject*& str) const
{
	return world_bbox.Distance(App->editorcamera->GetPosition()) < str->world_bbox.Distance(App->editorcamera->GetPosition());
}

float4x4 GameObject::getInvertedTransform()
{
	return  Transform()->getWorldTransform().Inverted();
}

std::vector<math::Triangle> GameObject::getTriangles()
{
	std::vector<math::Triangle> tris = {};
	for (Component* c : components)
	{
		std::vector<math::Triangle> tris_aux = {};
		c->getTriangles(tris_aux);
		tris.insert(tris.end(), tris_aux.begin(), tris_aux.end());
	}
	return tris;
}