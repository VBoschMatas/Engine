#pragma once
#include "Component.h"
#include "Geometry/Frustum.h"

class ComponentCamera : public Component
{
public:
	ComponentCamera();
	~ComponentCamera() = default;

	float field_of_view;
	float near_distance;
	float far_distance;

	void Update(unsigned int program, float3& position, Quat& rotation, float3& scale) override;
	void DebugDraw() override;
	void printComponentInfo() override;

	Frustum getFrustum() { return frustum; };
private:
	void setFOV(float deg);

	Frustum frustum;
	float aspect_ratio;
	float3 look_direction;
};

