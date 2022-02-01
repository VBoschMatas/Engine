#include "Application.h"
#include "Geometry/Frustum.h"
#include "ModuleEditorCamera.h"
#include "GL/glew.h"
#include "Scene.h"
#include "Model.h"

Scene::Scene(unsigned int _id)
{
	name = "New Scene";
	id = _id;
	last_go_id = 0;
	last_material_id = 0;
	last_mesh_id = 0;
	GenerateQuadtree();
};

Scene::Scene(const char* _name, unsigned int _id) {
	name = _name;
	id = _id;
	last_go_id = 0;
	last_material_id = 0;
	last_mesh_id = 0;
};

void Scene::Load()
{
	GenerateQuadtree();

	last_go_id = 0;
	last_material_id = 0;
	last_mesh_id = 0;

	meshes = {};
	game_objects = {};
	scene_textures = {};
	scene_materials = {};

	ambient_light = AmbientLight();
}

void Scene::UpdateTransform()
{
	for (int i = 0; i < children.size(); ++i)
	{
		if (children[i] == nullptr) break;
		children[i]->UpdateTransform();
	}
}

void Scene::UpdateLights(unsigned int program)
{
	current_pointlight = 0;
	current_spotlight = 0;

	ambient_light.Update(program);

	for (int i = 0; i < children.size(); ++i)
	{
		if (children[i] == nullptr) break;
		children[i]->UpdateLights(program);
	}

	glUniform1i(glGetUniformLocation(program, "n_points"), current_pointlight);
	glUniform1i(glGetUniformLocation(program, "n_spots"), current_spotlight);
}

void Scene::UpdateBoundingBox()
{
	for (int i = 0; i < children.size(); ++i)
	{
		if (children[i] == nullptr) break;
		children[i]->UpdateBoundingBox();
	}
}

void Scene::Update(unsigned int program)
{
	UpdateTransform();
	UpdateLights(program);
	UpdateBoundingBox();

	Culling();

	for (int i = 0; i < children.size(); ++i)
	{
		if (children[i] == nullptr) break;
		children[i]->Update(program);
	}
}

void Scene::Culling()
{
	Frustum frustum;
	if (camera_culling == nullptr)
		frustum = App->editorcamera->getFrustum();
	else
		frustum = camera_culling->getFrustum();

	std::vector<GameObject*> quad_objects = {};
	quadtree.CollectIntersections(quad_objects, frustum);

	for (GameObject* go : quad_objects)
	{
		go->render = true;
	}
}

void Scene::GenerateQuadtree()
{
	quadtree.SetBoundaries(AABB(float3(-500, -30, -500), float3(500, 30, 500)));

	for (GameObject* go : game_objects)
	{
		if (go->world_bbox.IsFinite())
			quadtree.Insert(go);
	}
}


GameObject* Scene::AddGameObject(const std::string file_name, GoType type)
{
	return AddGameObject(file_name, nullptr, type);
}

GameObject* Scene::AddGameObject(const std::string file_name, GameObject* parent, GoType type)
{
	++last_go_id;
	GameObject* go = new GameObject(last_go_id, parent);

	go->Load(file_name, type);
	game_objects.push_back(go);

	if(go->world_bbox.IsFinite())
		quadtree.Insert(go);

	if(parent == nullptr)
		children.push_back(go);

	return go;
}

void Scene::AddGameObjectIntoQuadtree(GameObject* gameobject)
{
	if (gameobject->world_bbox.IsFinite())
	{
		quadtree.Erase(gameobject);
		quadtree.Insert(gameobject);
	}
}

void Scene::RemoveGameObjectFromQuadtree(GameObject* gameobject)
{
	quadtree.Erase(gameobject);
}

void Scene::NewTexture(const char* path)
{
	unsigned int tex_hash = std::hash<std::string>{}(path);
	std::map<unsigned int, Texture>::iterator it = GetTextures()->find(tex_hash);
	std::map<unsigned int, Texture>::iterator end_it = GetTextures()->end();
	if (it != end_it)
	{
		return;
	}

	Texture texture;
	bool temp;
	texture = App->textures->LoadTexture(path, temp);
	AddTexture(tex_hash, texture);
}

GameObject* Scene::getGameObject(unsigned int id)
{ 
	auto it = std::find_if(game_objects.begin(), game_objects.end(), [&id](GameObject* obj) {return obj->getID() == id; });
	if (it != game_objects.end())
		return *it;
	else
		return nullptr;
}

void Scene::printHierarchy()
{

	if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{

		static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

		static int selection_mask = (1 << 2);

		for (int i = 0; i < children.size(); ++i)
		{
			if (children[i] == nullptr) break;
			children[i]->printHierarchy(base_flags);
		}

		/*if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject"))
			{
				IM_ASSERT(payload->DataSize == sizeof(GameObject*));
				GameObject* payload_n = (GameObject*)payload->Data;
				payload_n->getParent()->removeChild(payload_n);
				payload_n->setParent(nullptr);
			}
			ImGui::EndDragDropTarget();
		}*/

		if (selected_gameObject != nullptr)
		{
			selected_gameObject->selected = true;
		}
	}
}