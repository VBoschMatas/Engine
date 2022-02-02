#include "MeshImporter.h"
#include "ComponentMesh.h"
#include "assimp/mesh.h"

void MeshImporter::Import(const aiMesh* mesh, Mesh* ourObject) const
{
	console->AddLog("Importing mesh...");
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<float3> comb_vertices;
	console->AddLog("	Getting vertices");
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;
		float3 p_vector;

		p_vector.x = mesh->mVertices[i].x;
		p_vector.y = mesh->mVertices[i].y;
		p_vector.z = mesh->mVertices[i].z;
		vertex.position = p_vector;
		comb_vertices.push_back(p_vector);
		p_vector.x = mesh->mNormals[i].x;
		p_vector.y = mesh->mNormals[i].y;
		p_vector.z = mesh->mNormals[i].z;
		vertex.normal = p_vector;

		if (mesh->mTextureCoords[0])
		{
			float2 t_vector;
			t_vector.x = mesh->mTextureCoords[0][i].x;
			t_vector.y = mesh->mTextureCoords[0][i].y;
			vertex.uv = t_vector;
		}
		else
			vertex.uv = float2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}
	console->AddLog("	Getting indices");
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j)
		{
			indices.push_back(face.mIndices[j]);
		}
	}
}

std::uint64_t MeshImporter::Save(const Mesh* ourObject, char** fileBuffer) const
{
	return 0;
}

void MeshImporter::Load(const char* fileBuffer, Mesh* ourObject) const
{

}