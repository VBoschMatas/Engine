#pragma once
#include "Component.h"
#include "MathGeoLib.h"

#define RADTODEG 180/math::pi

class ComponentTransform : public Component
{
public:
	ComponentTransform();
	~ComponentTransform() {};

	void UpdateTransform(float3& position, Quat& rotation, float3& scale) override;
	void printComponentInfo() override;

	float3 getPos() { return float3(position[0], position[1], position[2]); };
	Quat getRot() { return Quat::FromEulerXYZ(rotation[0], rotation[1], rotation[2]); };
	float3 getSca() { return float3(scale[0], scale[1], scale[2]); };

	float3 getWorldPos() { return w_position; };
	Quat getWorldRot() { return w_rotation; };
	float3 getWorldSca() { return w_scale; };

	float4x4 getWorldTransform();
	float4x4 getLocalTransform();

	void setPos(float x, float y, float z) { position[0] = x; position[1] = y; position[2] = z; };
	void setPos(float3 xyz) { setPos(xyz.x, xyz.y, xyz.z); };
	void setRot(float x, float y, float z) { rotation[0] = x; rotation[1] = y; rotation[2] = z; };
	void setRot(float3 xyz) { setRot(xyz.x, xyz.y, xyz.z); };
	void setRot(Quat quat) { setRot(quat.ToEulerXYZ() * RADTODEG); };
	void setRot(float x, float y, float z, float w);
	void setSca(float x, float y, float z) { scale[0] = x; scale[1] = y; scale[2] = z; };
	void setSca(float3 xyz) { setSca(xyz.x, xyz.y, xyz.z); };
	void setTransform(float4x4 _transform);

private:
	float position [3];
	float rotation [3];
	float scale [3];

	float3 w_position;
	Quat w_rotation;
	float3 w_scale;
};

