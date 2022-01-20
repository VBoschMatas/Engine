#include "GameObject.h"
#include "Model.h"

GameObject::GameObject(unsigned int _id)
{
	id = _id;
	name = "EmptyObject";
	parent = nullptr;
};

void GameObject::Load(std::string file_name, GoType _type)
{
	name = file_name;
	type = _type;

	switch (type)
	{
	case GoType::Model:
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

}