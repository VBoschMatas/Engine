#pragma once
#include "Globals.h"

class Material;
struct aiMaterial;

class MaterialImporter
{
public:
	MaterialImporter() {};
	~MaterialImporter() {};
	static void Import(const aiMaterial* object, Material* ourObject);
	static std::uint64_t Save(const Material* ourObject, char** fileBuffer);
	static void Load(const char* fileBuffer, Material* ourObject);
};

