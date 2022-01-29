#include "ComponentMesh.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleProgram.h"
#include "ModuleEditorCamera.h"
#include "ModuleDebugDraw.h"
#include "GL/glew.h"
#include "Math/float2.h"
#include "Math/float4x4.h"
#include <array>

#include "debugdraw.h"

ComponentMesh::ComponentMesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices, Material* material, const char* name, const std::vector<float3>& obb_vertices, unsigned int id)
{
	this->id = id;
	mesh = new Mesh(vertices, indices, material, name, obb_vertices);
	App->scene->getCurrentScene()->AddMesh(mesh);
	console->AddLog("NUMBER OF Indices: %d", indices.size());
	type = CompType::Mesh;
	visible = true;
}

ComponentMesh::~ComponentMesh()
{}

void ComponentMesh::Update(unsigned int program, float3& position, Quat& rotation, float3& scale)
{
	if (!visible)
		return;

	const float4x4& view = App->editorcamera->getView();
	const float4x4 proj = App->editorcamera->getProjection();
	float4x4 model = float4x4::FromTRS(position, rotation, scale);

	/*std::vector<math::Triangle> tris = mesh->getTriangles();
	for (math::Triangle const& tri : tris)
	{
		dd::line(tri.a, tri.b, dd::colors::Yellow);
		dd::line(tri.b, tri.c, dd::colors::Yellow);
		dd::line(tri.c, tri.a, dd::colors::Yellow);
	}*/

	glUseProgram(program);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, (const float*)&model);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, (const float*)&view);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, (const float*)&proj);

	for (unsigned int i = 0; i < 4; ++i)
	{
		if (mesh->getMaterial()->getTextures()[i] != nullptr)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glUniform1i(glGetUniformLocation(program, mesh->getMaterial()->getTexTypes()[i]), 0);
			glBindTexture(GL_TEXTURE_2D, mesh->getMaterial()->getTextures()[i]->id);
		}
	}

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(mesh->getVAO());
	glDrawElements(GL_TRIANGLES, mesh->getIndicesNum(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void ComponentMesh::getBoundingBox(math::AABB& bbox)
{
	bbox = mesh->getBoundingBox();
}

void ComponentMesh::printComponentInfo()
{
	const ImVec4 yellow_colour(255, 255, 0, 255);
	static ImGuiTreeNodeFlags header_flags = ImGuiTreeNodeFlags_DefaultOpen;
	std::string itemid = "##" + std::to_string(this->id);
	if (ImGui::CollapsingHeader(std::string("Mesh" + itemid).c_str(), header_flags))
	{
		ImGui::TextColored(yellow_colour, "Triangles: "); ImGui::SameLine();
		ImGui::Text("%d  (V: %d  I: %d)", GetIndices() / 3, GetVertices(), GetIndices());

		ImGui::TextColored(yellow_colour, "Using material: "); ImGui::SameLine();
		ImGui::Text(mesh->getMaterial()->name.c_str());

		ImGui::Checkbox(std::string("Visible" + itemid).c_str(), &visible);
	}
}

void ComponentMesh::getTriangles(std::vector<math::Triangle> &triangles)
{
	triangles = mesh->getTriangles();
}

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, Material* material, const char* name, const std::vector<float3>& obb_vertices)
{
	this->vertices = vertices;
	this->indices = indices;
	this->material_index = material;
	this->name = name;
	this->id = App->scene->getScene(App->scene->current_scene)->getMeshId();
	console->AddLog("Creating OBB for the mesh");

	bounding_box.SetNegativeInfinity();
	bounding_box.Enclose(&obb_vertices[0], obb_vertices.size());

	LoadVBO();
	LoadEBO();
	CreateVAO();
}

void Mesh::LoadVBO()
{
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	console->AddLog("	VBO loaded");

	num_vertices = vertices.size();
}

void Mesh::LoadEBO()
{
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	console->AddLog("	EBO loaded");

	num_indices = indices.size();
}

void Mesh::CreateVAO()
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

std::vector<math::Triangle> Mesh::getTriangles()
{
	std::vector<math::Triangle> tris = {};

	for (int i = 0; i < indices.size(); i += 3)
	{
		float3 p_1 = vertices[indices[i]].position;
		float3 p_2 = vertices[indices[i + 1]].position;
		float3 p_3 = vertices[indices[i + 2]].position;
		tris.push_back(math::Triangle(p_1, p_2, p_3));
	}
	return tris;
}