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

	glUseProgram(program);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, (const float*)&model);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, (const float*)&view);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, (const float*)&proj);

	/*for (unsigned int i = 0; i < mesh->getMaterial()->getTextures().size(); ++i)
	{*/
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(program, "diffuse"), 0);
		glBindTexture(GL_TEXTURE_2D, mesh->getMaterial()->getTextures()[0]->id);
	//}

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

		ImGui::Checkbox(std::string("Visible" + itemid).c_str(), &visible);
	}
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
