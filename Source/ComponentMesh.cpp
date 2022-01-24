#include "ComponentMesh.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleEditorCamera.h"
#include "GL/glew.h"
#include "Math/float2.h"
#include "Math/float4x4.h"

ComponentMesh::ComponentMesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, std::vector<Texture> &textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	type = CompType::Mesh;
	visible = true;
	LoadVBO();
	LoadEBO();
	CreateVAO();
	console->AddLog("Base mesh component of type: %d", getType());
}

ComponentMesh::~ComponentMesh()
{
	//glDeleteBuffers(1, &ebo);
	//glDeleteBuffers(1, &vbo);
	//glDeleteVertexArrays(1, &vao);
}

void ComponentMesh::LoadVBO()
{
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	console->AddLog("	VBO loaded");
	num_vertices = vertices.size();
}

void ComponentMesh::LoadEBO()
{
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	console->AddLog("	EBO loaded");
	num_indices = indices.size();
}

void ComponentMesh::CreateVAO()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

	glBindVertexArray(0);
	console->AddLog("	VAO created with VBO and EBO");
}

void ComponentMesh::Update(unsigned int program, float3& position, float3& rotation, float3& scale)
{
	if (!visible)
		return;

	const float4x4& view = App->editorcamera->getView();
	const float4x4 proj = App->editorcamera->getProjection();
	float4x4 model = float4x4::Translate(position.x, position.y, position.z);

	glUseProgram(program);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, (const float*)&model);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, (const float*)&view);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, (const float*)&proj);
	for (unsigned int i = 0; i < textures.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glUniform1i(glGetUniformLocation(program, "diffuse"), 0);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void ComponentMesh::printComponentInfo()
{
	const ImVec4 title_colour(255, 255, 0, 255);

	ImGui::TextColored(title_colour, "Mesh");

	ImGui::TextWrapped("Triangles: %d  (V: %d  I: %d)", GetIndices() / 3, GetVertices(), GetIndices());

	ImGui::Checkbox("Visible", &visible);
}