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

	virtual void Update(unsigned int program, float3 &position, float3 &rotation, float3 &scale) {};
	
	virtual void PrintComponentInfo()
	{
		ImGui::TextColored(ImVec4 (255, 255, 0, 255), "Empty Component");
	};

private:
	unsigned int id;
	CompType type;
};