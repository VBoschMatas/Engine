#include "AmbientLight.h"
#include "GL/glew.h"

AmbientLight::AmbientLight()
{
	color = float3(1.0f, 0.4f, 0.4f);
	strength = 0.4f;
}

void AmbientLight::Update(unsigned int program)
{
	glUniform3fv(glGetUniformLocation(program, "ambientColor"), 1, (const float*)&color);
	glUniform1f(glGetUniformLocation(program, "ambientStrength"), strength);
}

//bool active = true;
//bool selected = false;

void AmbientLight::printGameObjectInfo()
{
	ImGui::ColorEdit3("Color", (float*)&color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float);
	ImGui::SliderFloat("Color strength", &strength, 0.0f, 1.0f);
}