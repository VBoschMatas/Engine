#include "ComponentTransform.h"
#include "Math/float4x4.h"
#include <math.h>

#define RADTODEG 180/math::pi
#define DEGTORAD math::pi/180

ComponentTransform::ComponentTransform()
{
	type = CompType::Transform;
	position[0] = 0.0f; position[1] = 0.0f; position[2] = 0.0f;
	rotation[0] = 0.0f; rotation[1] = 0.0f; rotation[2] = 0.0f;
	scale[0] = 1.0f; scale[1] = 1.0f; scale[2] = 1.0f;
}

void ComponentTransform::UpdateTransform(float3& position, Quat& rotation, float3& scale)
{
	position = { position.x + this->position[0], position.y + this->position[1], position.z + this->position[2] };
	float3 eulerRotation = rotation.ToEulerXYZ() * RADTODEG;
	eulerRotation = { eulerRotation.x + this->rotation[0], eulerRotation.y + this->rotation[1], eulerRotation.z + this->rotation[2] };
	eulerRotation = eulerRotation * DEGTORAD;
	rotation = Quat::FromEulerXYZ(eulerRotation.x, eulerRotation.y, eulerRotation.z);
	scale = { scale.x * this->scale[0], scale.y * this->scale[1], scale.z * this->scale[2] };
	w_position = position;
	w_rotation = rotation;
	w_scale = scale;
}

void ComponentTransform::setRot(float x, float y, float z, float w)
{
	Quat t_quat(x, y, z, w);
	float3 eulerRotation = t_quat.ToEulerXYZ()* RADTODEG;
	setRot(eulerRotation);
}

void ComponentTransform::printComponentInfo()
{
	static ImGuiTreeNodeFlags header_flags = ImGuiTreeNodeFlags_DefaultOpen;
	if (ImGui::CollapsingHeader("Transform", header_flags))
	{
		ImGui::DragFloat3("Position", position, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f", 1.0f);
		ImGui::DragFloat3("Rotation", rotation, 0.1f, -FLT_MAX, +FLT_MAX, "%.3f", 1.0f);
		ImGui::DragFloat3("Scale", scale, 0.001f, -FLT_MAX, +FLT_MAX, "%.3f", 1.0f);
	}
}

float4x4 ComponentTransform::getWorldTransform()
{
	return float4x4::FromTRS(w_position, w_rotation, w_scale);
}

float4x4 ComponentTransform::getLocalTransform()
{
	return float4x4::FromTRS(getPos(), getRot(), getSca());
}
