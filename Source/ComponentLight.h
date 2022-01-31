#pragma once
#include "Component.h"

enum class LightType
{
	Directional,
	Point,
	Spot
};

class ComponentLight : public Component
{

public:
	ComponentLight(LightType _type);
	~ComponentLight() = default;

	void UpdateLight(unsigned int program, const float3& position, const Quat& rotation, const float3& scale) override;
	void Update(unsigned int program, const float3& position, const Quat& rotation, const float3& scale) override;

	void printComponentInfo() override;

	float3 color;
	LightType Ltype;
private:
	void UpdateDirectional(unsigned int program, const float3& position, const Quat& rotation, const float3& scale);
	void UpdatePoint(unsigned int program, const float3& position, const Quat& rotation, const float3& scale);
	void UpdateSpot(unsigned int program, const float3& position, const Quat& rotation, const float3& scale);

	unsigned int id;
	float constant;
	float linear;
	float quadratic;
	float intensity;
	float innerAngle;
	float outerAngle;
};

