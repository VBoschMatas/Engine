#include "ComponentLight.h"
#include "Application.h"
#include "GL/glew.h"
#include "debugdraw.h"
#include "ModuleEditorCamera.h"
#include "ModuleEditor.h"

#include "Math/float4x4.h"
#include "Math/Quat.h"

#define DEGTORAD pi / 180.0f
#define RADTODEG 180.0f / pi

ComponentLight::ComponentLight(LightType _type)
{
	type = CompType::Light;
	Ltype = _type;
	color = float3(1.0f, 1.0f, 1.0f);
	intensity = 1.0f;
}
//
void ComponentLight::UpdateLight(unsigned int program, const float3& position, const Quat& rotation, const float3& scale)
{
	switch (Ltype)
	{
		case LightType::Directional:
		{
			UpdateDirectional(program, position, rotation, scale);
		}
		break;
		case LightType::Point:
		{
			UpdatePoint(program, position, rotation, scale);
		}
		break;
		case LightType::Spot:
		{
			UpdateSpot(program, position, rotation, scale);
		}
		break;
	}
}

void ComponentLight::Update(unsigned int program, const float3& position, const Quat& rotation, const float3& scale)
{
	if (!selected)
		return;

	switch (Ltype)
	{
	case LightType::Directional:
	{
		float3 dir = rotation * float3(0.0f, 0.0f, 1.0f);
		dd::circle(position, dir, dd::colors::Yellow, 1.0f, 16.0f);
		float3 points[8] = {
			float3(1.0f, 0.0f, 0.0f),
			float3(0.0f, 1.0f, 0.0f),
			float3(-1.0f, 0.0f, 0.0f),
			float3(0.0f, -1.0f, 0.0f),
			float3(0.70710f, 0.70710f, 0.0f),
			float3(-0.70710f, -0.70710f, 0.0f),
			float3(-0.70710f, 0.70710f, 0.0f),
			float3(0.70710f, -0.70710f, 0.0f)
		};
		for (float3 p : points) {
			float3 offset = rotation * p;
			dd::line(position + offset, dir*5.0f + position + offset, dd::colors::Yellow);
		}
	}
	break;
	case LightType::Point:
	{
		UpdatePoint(program, position, rotation, scale);
	}
	break;
	case LightType::Spot:
	{
		UpdateSpot(program, position, rotation, scale);
	}
	break;
	}
}

void ComponentLight::UpdateDirectional(unsigned int program, const float3& position, const Quat& rotation, const float3& scale)
{
	float3 paint_color = color * intensity;
	glUniform3fv(glGetUniformLocation(program, "directionalColor"), 1, (const float*)&paint_color);
	float3 light_direction = rotation * float3(0.0f, 0.0f, 1.0f) * -1.0f;
	glUniform3fv(glGetUniformLocation(program, "directionalDir"), 1, (const float*)&light_direction);
	//console->AddLog("DIR  X %f  Y %f  Z %f", light_direction.x, light_direction.y, light_direction.z);
}

void ComponentLight::UpdatePoint(unsigned int program, const float3& position, const Quat& rotation, const float3& scale)
{

}

void ComponentLight::UpdateSpot(unsigned int program, const float3& position, const Quat& rotation, const float3& scale)
{

}

//void ComponentLight::DebugDraw()
//{
//	dd::cone(frustum.Pos(), frustum.Front(), dd::colors::White, 0.5f, 0.0f);
//	dd::frustum(frustum.ViewProjMatrix().Inverted(), dd::colors::Red);
//}

void ComponentLight::printComponentInfo()
{
	static ImGuiTreeNodeFlags header_flags = ImGuiTreeNodeFlags_DefaultOpen;
	if (ImGui::CollapsingHeader("Light", header_flags))
	{
		ImGui::ColorEdit3("Color", (float*)&color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float);
		ImGui::DragFloat("Intensity", &intensity, 0.05f, 0.0f, FLT_MAX, "%.3f", 1.0f);
	}
}