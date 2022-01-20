#pragma once
#include "Globals.h"
#include <vector>

enum class GoType
{
	Empty,
	Model,
	Camera,
	Text
};

class Component;

class GameObject
{
public:
	GameObject(unsigned int _id);
	~GameObject() = default;

	void Load(const std::string file_name = "", GoType _type = GoType::Empty);

	void Update(unsigned int program);

	// Setters
	void setPosition(const float _x, const float _y, const float _z) { position = { _x, _y, _z }; };
	void setRotation(const float _x, const float _y, const float _z) { rotation = { _x, _y, _z }; };
	void setScale(const float _x, const float _y, const float _z) { scale = { _x, _y, _z }; };
	void setName(const char* _name) { name = _name; };
	void addChild(GameObject* _gameobject) { children.push_back(_gameobject); };
	void setParent(GameObject* _parent) { parent = _parent; };
	void addComponent(Component* _component) { components.push_back(_component); };

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
	float3 getPosition() { return position; };
	float3 getRotation() { return rotation; };
	float3 getScale() { return scale; };
	std::string getName() { return name; };
	GoType getType() { return type; };
	GameObject* getParent() { return parent; };
	std::vector<GameObject*> getChildren() { return children; };

	bool active;

private:
	unsigned int id;
	float3 position;
	float3 rotation;
	float3 scale;
	std::string name;
	GoType type;

	GameObject* parent;
	std::vector<GameObject*> children;

	std::vector<Component*> components;
};

