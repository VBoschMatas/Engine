#pragma once
#include "Globals.h"
#include "Geometry/AABB.h"
#include "Geometry/OBB.h"
#include "Geometry/Triangle.h"
#include <vector>

enum class GoType
{
	Empty,
	Model,
	Camera,
	Text,
	DirectionalLight,
	PointLight,
	SpotLight,
	Cube
};

class Component;
class Scene;
class ComponentTransform;

class GameObject
{
public:
	GameObject(unsigned int _id, GameObject* _parent);
	~GameObject();

	void Load(const std::string &file_name = "", GoType _type = GoType::Empty);

	void UpdateTransform();
	void UpdateBoundingBox();
	void UpdateLights(unsigned int program);
	void Update(unsigned int program);
	void DebugDraw();

	// Setters
	void setName(const char* _name) { name = _name; };
	void addChild(GameObject* _gameobject) { children.push_back(_gameobject); };
	void addChildren(std::vector<GameObject*> _gameobjects) { children.insert(children.end(), _gameobjects.begin(), _gameobjects.end()); };
	void setParent(GameObject* _parent) { parent = _parent; };
	void addComponent(Component* _component) { components.push_back(_component); }; // If just one component is added
	void addComponent(std::vector<Component*> _components) {components.insert(components.end(),_components.begin(), _components.end()); }; // If a group of components are added

	ComponentTransform* Transform();

	// We remove from the list 
	void removeChild(const GameObject* _gameobject)
	{
		children.erase(std::find(children.begin(), children.end(), _gameobject));
	}

	void removeComponent(const Component* _component)
	{
		components.erase(std::find(components.begin(), components.end(), _component));
	}

	// Getters
	std::string getName() { return name; };
	GoType getType() { return type; };
	GameObject* getParent() { return parent; };
	std::vector<GameObject*> getChildren() { return children; };
	std::vector<Component*> getComponents() { return components; };

	bool active = true;
	bool selected = false;
	bool render = false;

	math::OBB world_bbox = {};

	unsigned int getCompId() { unsigned int temp_id = last_comp_id; ++last_comp_id; return temp_id; };

	void printGameObjectInfo();
	void printHierarchy(ImGuiTreeNodeFlags flags);

	float4x4 getInvertedTransform();
	std::vector<math::Triangle> getTriangles();

	bool operator > (const GameObject*& str) const;

	bool operator < (const GameObject*& str) const;
private:

	unsigned int id;
	std::string name;
	GoType type;

	math::AABB local_bbox = {};

	GameObject* parent;
	std::vector<GameObject*> children;
	std::vector<Component*> components;

	void LoadCube();

	unsigned int last_comp_id = 0;
};

