#include "AmbientLight.h"
#include "GL/glew.h"
#include "Archive.h"

AmbientLight::AmbientLight()
{
	color = float3(1.0f, 0.0f, 1.0f);
	strength = 0.4f;
}

void AmbientLight::Update(unsigned int program)
{
	glUseProgram(program);

	glUniform3fv(glGetUniformLocation(program, "ambientColor"), 1, (const float*)&color);
	glUniform1f(glGetUniformLocation(program, "ambientStrength"), strength);
}

//bool active = true;
//bool selected = false;

void AmbientLight::printGameObjectInfo()
{
	ImGui::ColorEdit3("Color", (float*)&color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float);
	ImGui::DragFloat("Intensity", &strength, 0.05f, 0.0f, FLT_MAX, "%.3f", 1.0f);
}

void AmbientLight::Save(Archive* archive)
{
	archive->json["AmbientLight"]["Color"] = { color.x, color.y, color.z };
	archive->json["AmbientLight"]["Strength"] = strength;
}