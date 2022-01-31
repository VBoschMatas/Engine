#include "ComponentLight.h"
#include "Application.h"
#include "GL/glew.h"
#include "debugdraw.h"
#include "ModuleEditorCamera.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"

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
	constant = 1.0f;
	linear = 0.0f;
	quadratic = 0.0f;
	innerAngle = 20.0f;
	outerAngle = 30.0f;
}

void ComponentLight::UpdateLight(unsigned int program, const float3& position, const Quat& rotation, const float3& scale)
{
	glUseProgram(program);

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
		float3 points[9] = {
			float3(1.0f, 0.0f, 0.0f),
			float3(0.0f, 1.0f, 0.0f),
			float3(0.0f, 0.0f, 1.0f),
			float3(0.70710f, 0.70710f, 0.0f),
			float3(0.70710f, 0.0f, 0.70710f),
			float3(0.0f, 0.70710f, 0.70710f),
			float3(-0.70710f, 0.70710f, 0.0f),
			float3(-0.70710f, 0.0f, 0.70710f),
			float3(0.0f, -0.70710f, 0.70710f)
		};
		for (float3 p : points) {
			dd::line(p * -20.0f + position, p * 20.0f + position, dd::colors::Yellow);
		}
	}
	break;
	case LightType::Spot:
	{

	}
	break;
	}
}

void ComponentLight::UpdateDirectional(unsigned int program, const float3& position, const Quat& rotation, const float3& scale)
{
	float3 paint_color = color * intensity;
	glUniform3fv(glGetUniformLocation(program, "dirLight.color"), 1, (const float*)&paint_color);
	float3 light_direction = rotation * float3(0.0f, 0.0f, 1.0f);
	glUniform3fv(glGetUniformLocation(program, "dirLight.direction"), 1, (const float*)&light_direction);
}

void ComponentLight::UpdatePoint(unsigned int program, const float3& position, const Quat& rotation, const float3& scale)
{
	std::string pointLight = "pointLights[" + std::to_string(App->scene->getCurrentPointLight()) + "].";
	float3 paint_color = color * intensity;
	glUniform3fv(glGetUniformLocation(program, std::string(pointLight + "color").c_str()), 1, (const float*)&paint_color);
	glUniform3fv(glGetUniformLocation(program, std::string(pointLight + "position").c_str()), 1, (const float*)&position);
	glUniform1f(glGetUniformLocation(program, std::string(pointLight + "constant").c_str()), constant);
	glUniform1f(glGetUniformLocation(program, std::string(pointLight + "linear").c_str()), linear);
	glUniform1f(glGetUniformLocation(program, std::string(pointLight + "quadratic").c_str()), quadratic);
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
		ImGui::DragFloat("Intensity", &intensity, 0.005f, 0.0f, FLT_MAX, "%.3f", 1.0f);
		if (Ltype == LightType::Point || Ltype == LightType::Spot)
		{
			ImGui::Separator();
			ImGui::Text("Attenuation");
			ImGui::DragFloat("Constant", &constant, 0.005f, 0.005f, FLT_MAX, "%.3f", 1.0f);
			ImGui::DragFloat("Linear", &linear, 0.005f, 0.0f, FLT_MAX, "%.3f", 1.0f);
			ImGui::DragFloat("Quadratic", &quadratic, 0.005f, 0.0f, FLT_MAX, "%.3f", 1.0f);
			if (Ltype == LightType::Spot)
			{
				ImGui::Separator();
				ImGui::Text("Cutoff");
				ImGui::SliderFloat("Inner", &innerAngle, 0.0f, 90.0f, "%.0f", 1.0f);
				ImGui::SliderFloat("Outer", &outerAngle, 0.0f, 90.0f, "%.0f", 1.0f);
			}
		}
	}
}