#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "ComponentLight.h"
#include "Model.h"
#include "imgui.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "ModuleEditorCamera.h"
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
	App->scene->RemoveGameObjectFromQuadtree(this);
	for (GameObject* ch : children)
		delete(ch);
	for (Component* c : components)
		delete(c);
	if (parent != nullptr)
		parent->removeChild(this);
	if (App->scene->getSelectedGameObject() == this)
		App->scene->setSelectedGameObject(nullptr);
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
	{
		name = "Camera";
		ComponentCamera* camera = new ComponentCamera();
		components.push_back(camera);
	}
	break;
	case GoType::DirectionalLight:
	{
		name = "Light";
		ComponentLight* light = new ComponentLight(LightType::Directional);
		components.push_back(light);
	}
	break;
	case GoType::PointLight:
	{
		name = "Light";
		ComponentLight* light = new ComponentLight(LightType::Point);
		components.push_back(light);
	}
	break;
	case GoType::SpotLight:
	{
		name = "Light";
		ComponentLight* light = new ComponentLight(LightType::Spot);
		components.push_back(light);
	}
	break; case GoType::Cube:
	{
		name = "Cube";
		LoadCube();
	}
	break;
	case GoType::Text:
		name = "Text";
		break;
	default:
		console->AddLog("Creating empty object...");
		break;
	}


	for (Component* c : components)
	{
		c->getBoundingBox(local_bbox);
	}

	if (local_bbox.IsFinite())
	{
		world_bbox = local_bbox;
		world_bbox.Transform(Transform()->getWorldTransform());
	}
}

void GameObject::UpdateTransform()
{
	selected = App->scene->getSelectedGameObject() == this;

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
		c->render = render;
		c->UpdateTransform(position, rotation, scale);
	}

	for (GameObject* ch : children)
	{
		ch->UpdateTransform();
	}
}

void GameObject::UpdateBoundingBox()
{

	for (Component* c : components)
	{
		c->getBoundingBox(local_bbox);
	}

	for (GameObject* ch : children)
	{
		ch->UpdateBoundingBox();
		if (!active)
		{
			App->scene->RemoveGameObjectFromQuadtree(ch);
		}
	}

	// Bounding box Transform
	if (local_bbox.IsFinite())
	{
		world_bbox = local_bbox;
		world_bbox.Transform(Transform()->getWorldTransform());
	}

	if (!active)
	{
		App->scene->RemoveGameObjectFromQuadtree(this);
		return;
	}

	App->scene->AddGameObjectIntoQuadtree(this);
}

void GameObject::UpdateLights(unsigned int program)
{
	float3 position = Transform()->getWorldPos();
	Quat rotation = Transform()->getWorldRot();
	float3 scale = Transform()->getWorldSca();

	for (Component* c : components)
	{
		c->UpdateLight(program, position, rotation, scale);
	}

	for (GameObject* ch : children)
	{
		ch->UpdateLights(program);
	}
}

void GameObject::Update(unsigned int program)
{
	if (!active)
		return;

	float3 position = Transform()->getWorldPos();
	Quat rotation = Transform()->getWorldRot();
	float3 scale = Transform()->getWorldSca();

	for (Component* c : components)
	{
		c->selected = selected;
		c->render = render;
		c->Update(program, position, rotation, scale);
	}

	for (GameObject* ch : children)
	{
		if(selected)
			ch->selected = true;
		ch->Update(program);
	}

	if (active && App->editor->isDebugDraw())
		DebugDraw();

	render = false;
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

		if ((ImGui::IsItemClicked(ImGuiMouseButton_Left) || ImGui::IsItemClicked(ImGuiMouseButton_Right)) && !ImGui::IsItemToggledOpen())
			App->scene->setSelectedGameObject(this);

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
			App->scene->setSelectedGameObject(this);


	}
}

void GameObject::DebugDraw()
{
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

void GameObject::LoadCube()
{
	float3 positions [] = {
		float3(-0.5f, -0.5f, -0.5f),
		float3(0.5f, -0.5f, -0.5f),
		float3(0.5f,  0.5f, -0.5f),
		float3(0.5f,  0.5f, -0.5f),
		float3(-0.5f,  0.5f, -0.5f),
		float3(-0.5f, -0.5f, -0.5f),

		float3(-0.5f, -0.5f,  0.5f),
		float3(0.5f, -0.5f,  0.5f),
		float3(0.5f,  0.5f,  0.5f),
		float3(0.5f,  0.5f,  0.5f),
		float3(-0.5f,  0.5f,  0.5f),
		float3(-0.5f, -0.5f,  0.5f),

		float3(-0.5f,  0.5f,  0.5f),
		float3(-0.5f,  0.5f, -0.5f),
		float3(-0.5f, -0.5f, -0.5f),
		float3(-0.5f, -0.5f, -0.5f),
		float3(-0.5f, -0.5f,  0.5f),
		float3(-0.5f,  0.5f,  0.5f),

		float3(0.5f,  0.5f,  0.5f),
		float3(0.5f,  0.5f, -0.5f),
		float3(0.5f, -0.5f, -0.5f),
		float3(0.5f, -0.5f, -0.5f),
		float3(0.5f, -0.5f,  0.5f),
		float3(0.5f,  0.5f,  0.5f),

		float3(-0.5f, -0.5f, -0.5f),
		float3(0.5f, -0.5f, -0.5f),
		float3(0.5f, -0.5f,  0.5f),
		float3(0.5f, -0.5f,  0.5f),
		float3(-0.5f, -0.5f,  0.5f),
		float3(-0.5f, -0.5f, -0.5f),

		float3(-0.5f,  0.5f, -0.5f),
		float3(0.5f,  0.5f, -0.5f),
		float3(0.5f,  0.5f,  0.5f),
		float3(0.5f,  0.5f,  0.5f),
		float3(-0.5f,  0.5f,  0.5f),
		float3(-0.5f,  0.5f, -0.5f),
	};

	unsigned int ind[] = {
		5, 4, 3, 2, 1, 0,
		6, 7, 8, 9, 10, 11,
		12, 13, 14, 15, 16, 17,
		23, 22, 21, 20, 19, 18,
		24, 25, 26, 27, 28, 29,
		35, 34, 33, 32, 31, 30
	};

	float3 normals[] = {
		float3(0.0f,  0.0f, -1.0f),
		float3(0.0f,  0.0f, -1.0f),
		float3(0.0f,  0.0f, -1.0f),
		float3(0.0f,  0.0f, -1.0f),
		float3(0.0f,  0.0f, -1.0f),
		float3(0.0f,  0.0f, -1.0f),

		float3(0.0f,  0.0f,  1.0f),
		float3(0.0f,  0.0f,  1.0f),
		float3(0.0f,  0.0f,  1.0f),
		float3(0.0f,  0.0f,  1.0f),
		float3(0.0f,  0.0f,  1.0f),
		float3(0.0f,  0.0f,  1.0f),

		float3(1.0f,  0.0f,  0.0f),
		float3(1.0f,  0.0f,  0.0f),
		float3(1.0f,  0.0f,  0.0f),
		float3(1.0f,  0.0f,  0.0f),
		float3(1.0f,  0.0f,  0.0f),
		float3(1.0f,  0.0f,  0.0f),

		float3(1.0f,  0.0f,  0.0f),
		float3(1.0f,  0.0f,  0.0f),
		float3(1.0f,  0.0f,  0.0f),
		float3(1.0f,  0.0f,  0.0f),
		float3(1.0f,  0.0f,  0.0f),
		float3(1.0f,  0.0f,  0.0f),

		float3(0.0f, -1.0f,  0.0f),
		float3(0.0f, -1.0f,  0.0f),
		float3(0.0f, -1.0f,  0.0f),
		float3(0.0f, -1.0f,  0.0f),
		float3(0.0f, -1.0f,  0.0f),
		float3(0.0f, -1.0f,  0.0f),

		float3(0.0f,  1.0f,  0.0f),
		float3(0.0f,  1.0f,  0.0f),
		float3(0.0f,  1.0f,  0.0f),
		float3(0.0f,  1.0f,  0.0f),
		float3(0.0f,  1.0f,  0.0f),
		float3(0.0f,  1.0f,  0.0f),
	};

	float2 uv[] = {
		float2(0.0f,  0.0f),
		float2(1.0f,  0.0f),
		float2(1.0f,  1.0f),
		float2(1.0f,  1.0f),
		float2(0.0f,  1.0f),
		float2(0.0f,  0.0f),

		float2(0.0f,  0.0f),
		float2(1.0f,  0.0f),
		float2(1.0f,  1.0f),
		float2(1.0f,  1.0f),
		float2(0.0f,  1.0f),
		float2(0.0f,  0.0f),

		float2(1.0f,  0.0f),
		float2(1.0f,  1.0f),
		float2(0.0f,  1.0f),
		float2(0.0f,  1.0f),
		float2(0.0f,  0.0f),
		float2(1.0f,  0.0f),

		float2(1.0f,  0.0f),
		float2(1.0f,  1.0f),
		float2(0.0f,  1.0f),
		float2(0.0f,  1.0f),
		float2(0.0f,  0.0f),
		float2(1.0f,  0.0f),

		float2(0.0f,  1.0f),
		float2(1.0f,  1.0f),
		float2(1.0f,  0.0f),
		float2(1.0f,  0.0f),
		float2(0.0f,  0.0f),
		float2(0.0f,  1.0f),

		float2(0.0f,  1.0f),
		float2(1.0f,  1.0f),
		float2(1.0f,  0.0f),
		float2(1.0f,  0.0f),
		float2(0.0f,  0.0f),
		float2(0.0f,  1.0f),
	};

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<float3> comb_vertices;

	for (unsigned int i = 0; i < 36; ++i)
	{
		Vertex vertex;
		float3 p_vector;

		p_vector.x = positions[i].x;
		p_vector.y = positions[i].y;
		p_vector.z = positions[i].z;
		vertex.position = p_vector;
		comb_vertices.push_back(p_vector);

		p_vector.x = normals[i].x;
		p_vector.y = normals[i].y;
		p_vector.z = normals[i].z;
		vertex.normal = p_vector;

		float2 t_vector;
		t_vector.x = uv[i].x;
		t_vector.y = uv[i].y;
		vertex.uv = t_vector;

		vertices.push_back(vertex);

		indices.push_back(ind[i]);
	}
	ComponentMaterial* material = new ComponentMaterial(nullptr, 0, this->getCompId());
	material->setMaterial(new Material());
	ComponentMesh* c_mesh = new ComponentMesh(vertices, indices, material, "Cube", comb_vertices, this->getCompId());

	components.push_back(material);
	components.push_back(c_mesh);
}