#pragma once
#include "Globals.h"

class Mesh;
struct aiMesh;

class MeshImporter
{
	MeshImporter() {};
	~MeshImporter() {};
	void Import(const aiMesh* object, Mesh* ourObject) const;
	std::uint64_t Save(const Mesh* ourObject, char** fileBuffer) const;
	void Load(const char* fileBuffer, Mesh* ourObject) const;
};

