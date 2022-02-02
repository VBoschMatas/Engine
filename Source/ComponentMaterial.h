#pragma once
#include "Component.h"
#include "ModuleTexture.h"
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"
#include <vector>
class Archive;
class Material
{
public:
	Material();
	Material(aiMaterial* material);
	~Material() = default;

	Texture* getTexture(TexType type);
	Texture** getTextures() { return textures; };
	const char** getTexTypes() { return tex_types; };

	void setTexture(Texture* texture, unsigned int tex_id);

	void addTexture(const char* path, unsigned int tex_id);
	void RemoveTexture(unsigned int tex_id);

	unsigned int getId() { return id; };

	void Save();

	std::string name;
	float3 ambient;
	float3 diffuse;
	float3 specular;

	float metallic;
	float albedo;
	float smoothness;
	u32 uuid;
private:
	unsigned int id;
	//Texture* textures[4] = { nullptr, nullptr, nullptr, nullptr };
	Texture* textures[2] = { nullptr, nullptr };
	const char* tex_types[2] = { "diffuse", "specular"};
	//const char* tex_types[4] = { "diffuse", "normals", "specular", "lightmap"};
	Texture default_texture;
	Texture* LoadTextures(const char* path);

};

class ComponentMaterial : public Component
{
public:
	ComponentMaterial(const aiMesh* mesh, unsigned int offset, unsigned int id);
	virtual ~ComponentMaterial() = default;

	//void Update(unsigned int program, float3 & position, Quat & rotation, float3 & scale) override;

	void printComponentInfo() override;
	void Save(Archive* archive) override;
	void showImage(unsigned int i);

	void setMaterial(Material* _material) { material = _material; };
	Material* getMaterial() { return material; };

private:
	Material* material;

	void selectTexture(unsigned int tex_id);
};