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

void ComponentTransform::setTransform(float4x4 _transform)
{
	setPos(_transform[0][3], _transform[1][3], _transform[2][3]);

	Quat q;
	q.w = math::Sqrt(math::Max(0.0f, 1.0f + _transform[0][0] + _transform[1][1] + _transform[2][2])) / 2.0f;
	q.x = math::Sqrt(math::Max(0.0f, 1.0f + _transform[0][0] - _transform[1][1] - _transform[2][2])) / 2.0f;
	q.y = math::Sqrt(math::Max(0.0f, 1.0f - _transform[0][0] + _transform[1][1] - _transform[2][2])) / 2.0f;
	q.z = math::Sqrt(math::Max(0.0f, 1.0f - _transform[0][0] - _transform[1][1] + _transform[2][2])) / 2.0f;
	q.x *= math::Sign(q.x * (_transform[2][1] - _transform[1][2]));
	q.y *= math::Sign(q.y * (_transform[0][2] - _transform[2][0]));
	q.z *= math::Sign(q.z * (_transform[1][0] - _transform[0][1]));
	setRot(q.ToEulerXYZ() * RADTODEG);
	setSca(_transform[0][0], _transform[1][1], _transform[2][2]);

	//console->AddLog("POSITION %f  %f  %f", _transform.Col3(3).x, _transform.Col3(3).y, _transform.Col3(3).z);
	//console->AddLog("ROTATION %f  %f  %f", q.ToEulerXYZ().x * RADTODEG, q.ToEulerXYZ().y * RADTODEG, q.ToEulerXYZ().z * RADTODEG);
	//console->AddLog("SCALE %f  %f  %f", _transform[0][0], _transform[1][1], _transform[2][2]);

	console->AddLog("%f  %f  %f  %f", _transform[0][0], _transform[0][1], _transform[0][2], _transform[0][3]);
	console->AddLog("%f  %f  %f  %f", _transform[1][0], _transform[1][1], _transform[1][2], _transform[1][3]);
	console->AddLog("%f  %f  %f  %f", _transform[2][0], _transform[2][1], _transform[2][2], _transform[2][3]);
	console->AddLog("%f  %f  %f  %f", _transform[3][0], _transform[3][1], _transform[3][2], _transform[3][3]);
	_transform.ptr();
	console->AddLog("MATRIX: %s", _transform.SerializeToString());
}