#include "ComponentTransform.h"

ComponentTransform::ComponentTransform()
{
	type = CompType::Transform;
	position[0] = 0.0f; position[1] = 0.0f; position[2] = 0.0f;
	rotation[0] = 0.0f; rotation[1] = 0.0f; rotation[2] = 0.0f;
	scale[0] = 1.0f; scale[1] = 1.0f; scale[2] = 1.0f;
	console->AddLog("Base transform component of type: %d", getType());
}

void ComponentTransform::Update(unsigned int program, float3& position, float3& rotation, float3& scale)
{
	position = { position.x + this->position[0], position.y + this->position[1], position.z + this->position[2] };
	rotation = { rotation.x + this->rotation[0], rotation.y + this->rotation[1], rotation.z + this->rotation[2] };
	scale = { scale.x * this->scale[0], scale.y * this->scale[1], scale.z * this->scale[2] };
}

void ComponentTransform::printComponentInfo()
{
	const ImVec4 title_colour(255, 255, 0, 255);

	ImGui::TextColored(title_colour, "Transform");

	ImGui::DragFloat3("Position", position, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f", 1.0f);
	ImGui::DragFloat3("Rotation", rotation, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f", 1.0f);
	ImGui::DragFloat3("Scale", scale, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f", 1.0f);
}