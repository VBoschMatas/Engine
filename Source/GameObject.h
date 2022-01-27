#pragma once
#include "Globals.h"
#include "Geometry/AABB.h"
#include "Geometry/OBB.h"
#include <vector>

enum class GoType
{
	Empty,
	Model,
	Camera,
	Text,
	Light
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

	math::OBB getBoundingBox() { return world_bbox; };

	bool active;
	bool selected;

	unsigned int getCompId() { unsigned int temp_id = last_comp_id; ++last_comp_id; return temp_id; };

	void printGameObjectInfo();
	void printHierarchy(ImGuiTreeNodeFlags flags);
private:

	unsigned int id;
	std::string name;
	GoType type;

	math::AABB local_bbox;
	math::OBB world_bbox;

	GameObject* parent;
	std::vector<GameObject*> children;
	std::vector<Component*> components;

	unsigned int last_comp_id = 0;
};

