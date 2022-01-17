#pragma once

enum class CompType
{
	Transform,
	Mesh,
	Material
};

class Component
{
public:
	Component() = default;
	virtual ~Component() = default;

	unsigned int getId() { return id; };
	CompType getType() { return type; };

	virtual void Update();

private:
	unsigned int id;
	CompType type;
};