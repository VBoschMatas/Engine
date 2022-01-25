#pragma once
#include "Component.h"
#include "ModuleTexture.h"
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"
#include <vector>

class Material
{
public:
	Material(aiMaterial* material, const char* path, unsigned int id);
	~Material() = default;

	Texture* getTexture(TexType type);
	std::vector<Texture*> getTextures() { return textures; };
	unsigned int getId() { return id; };

private:
	unsigned int id;
	std::vector<Texture*> textures;
	Texture default_texture;
	std::vector<Texture*> LoadTextures(aiMaterial* material, aiTextureType type, const char* path);
};

class ComponentMaterial : public Component
{
public:
	ComponentMaterial(const aiMesh* mesh);
	virtual ~ComponentMaterial() = default;

	//void Update(unsigned int program, float3 & position, Quat & rotation, float3 & scale) override;

	void printComponentInfo() override;

	void setMaterial(Material* _material) { material = _material; };
	Material* getMaterial() { return material; };

private:
	Material* material;
};