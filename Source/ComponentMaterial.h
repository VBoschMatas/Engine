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
	Texture** getTextures() { return textures; };
	const char** getTexTypes() { return tex_types; };

	void setTexture(Texture* texture, unsigned int tex_id);

	void addTexture(const char* path, unsigned int tex_id);
	void RemoveTexture(unsigned int tex_id);

	unsigned int getId() { return id; };

	std::string name;

private:
	unsigned int id;
	Texture* textures[4] = { nullptr, nullptr, nullptr, nullptr };
	const char* tex_types[4] = { "diffuse", "normals", "specular", "lightmap"};
	Texture default_texture;
	Texture* LoadTextures(aiMaterial* material, aiTextureType type, const char* path);
};

class ComponentMaterial : public Component
{
public:
	ComponentMaterial(const aiMesh* mesh, unsigned int offset, unsigned int id);
	virtual ~ComponentMaterial() = default;

	//void Update(unsigned int program, float3 & position, Quat & rotation, float3 & scale) override;

	void printComponentInfo() override;
	void showImage(unsigned int i);

	void setMaterial(Material* _material) { material = _material; };
	Material* getMaterial() { return material; };

private:
	Material* material;

	void selectTexture(unsigned int tex_id);
};