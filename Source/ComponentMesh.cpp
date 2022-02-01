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
	App->scene->AddMesh(mesh);
	console->AddLog("NUMBER OF Indices: %d", indices.size());
	type = CompType::Mesh;
	visible = true;
}

ComponentMesh::~ComponentMesh()
{}

void ComponentMesh::Update(unsigned int program, const float3& position, const Quat& rotation, const float3& scale)
{
	if (!render || !visible)
		return;
	if (selected)
	{
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
	} 
	else
	{
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0x00);
	}

	Draw(program, position, rotation, scale);

	if (!selected)
		return;

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);

	glDisable(GL_DEPTH_TEST);
	Draw(App->program->outline_program, position, rotation, scale*1.02);
	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glEnable(GL_DEPTH_TEST);

}

void ComponentMesh::Draw(unsigned int program, const float3& position, const Quat& rotation, const float3& scale)
{
	const float4x4& view = App->editorcamera->getView();
	const float4x4 proj = App->editorcamera->getProjection();
	const float3 cam_pos = App->editorcamera->GetPosition();
	float4x4 model = float4x4::FromTRS(position, rotation, scale);

	glUseProgram(program);

	glUniform3fv(glGetUniformLocation(program, "viewPosition"), 1, (const float*)&cam_pos);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, (const float*)&model);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, (const float*)&view);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, (const float*)&proj);

	// Material properties
	glUniform3fv(glGetUniformLocation(program, "material.ambient"), 1, (const float*)&mesh->getMaterial()->ambient);
	glUniform3fv(glGetUniformLocation(program, "material.diffuse"), 1, (const float*)&mesh->getMaterial()->diffuse);
	glUniform3fv(glGetUniformLocation(program, "material.specular"), 1, (const float*)&mesh->getMaterial()->specular);

	glUniform1f(glGetUniformLocation(program, "material.metallic"), mesh->getMaterial()->metallic);
	glUniform1f(glGetUniformLocation(program, "material.albedo"), mesh->getMaterial()->albedo);
	glUniform1f(glGetUniformLocation(program, "material.smoothness"), mesh->getMaterial()->smoothness);

	glUniform1i(glGetUniformLocation(program, "material.diffuse_map"), 0);
	//glUniform1i(glGetUniformLocation(program, "material.normals_map"), 1);
	glUniform1i(glGetUniformLocation(program, "material.specular_map"), 1);
	//glUniform1i(glGetUniformLocation(program, "material.occlusion_map"), 3);

	for (unsigned int i = 0; i < 2; ++i)
	{
		std::string map = mesh->getMaterial()->getTexTypes()[i];

		if (mesh->getMaterial()->getTextures()[i] != nullptr)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, mesh->getMaterial()->getTextures()[i]->id);
		}
		std::string has_map = "material.has_" + map+ "_map";
		glUniform1i(glGetUniformLocation(program, has_map.c_str()), mesh->getMaterial()->getTextures()[i] != nullptr);
	}

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
	this->id = App->scene->getMeshId();
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