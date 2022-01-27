#pragma once
#include "Component.h"
#include "MathGeoLib.h"

class ComponentTransform : public Component
{
public:
	ComponentTransform();
	~ComponentTransform() {};

	void Update(unsigned int program, float3& position, Quat& rotation, float3& scale) override;
	void printComponentInfo() override;

	float3 getPos() { return float3(position[0], position[1], position[2]); };
	Quat getRot() { return Quat::FromEulerXYZ(rotation[0], rotation[1], rotation[2]); };
	float3 getSca() { return float3(scale[0], scale[1], scale[2]); };

	void setPos(float x, float y, float z) { position[0] = x; position[1] = y; position[2] = z; };
	void setPos(float3 xyz) { setPos(xyz.z, xyz.y, xyz.z); };
	void setRot(float x, float y, float z) { rotation[0] = x; rotation[1] = y; rotation[2] = z; };
	void setRot(float3 xyz) { setRot(xyz.z, xyz.y, xyz.z); };
	void setRot(float x, float y, float z, float w);
	void setSca(float x, float y, float z) { scale[0] = x; scale[1] = y; scale[2] = z; };
	void setSca(float3 xyz) { setRot(xyz.z, xyz.y, xyz.z); };

private:
	float position [3];
	float rotation [3];
	float scale [3];
};

