#pragma once
#include "Component.h"
#include "Geometry/Frustum.h"
class Archive;
class ComponentCamera : public Component
{
public:
	ComponentCamera();
	~ComponentCamera() = default;

	float field_of_view;
	float near_distance;
	float far_distance;

	void Update(unsigned int program, const float3& position, const Quat& rotation, const float3& scale) override;
	void DebugDraw() override;
	void printComponentInfo() override;
	void Save(Archive* archive) override;

	Frustum getFrustum() { return frustum; };
private:
	void setFOV(float deg);

	Frustum frustum;
	float aspect_ratio;
	float3 look_direction;
};

