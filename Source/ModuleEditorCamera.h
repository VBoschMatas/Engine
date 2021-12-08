#pragma once
#include "Module.h"
#include "Globals.h"
#include "MathGeoLib.h"

class ModuleEditorCamera: public Module
{
public:
	ModuleEditorCamera();
	~ModuleEditorCamera();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

private:
	void InitPerspectiveMatrix();
	void InitViewMatrix();

	void SetPosition(const float3& position);
	void LookAt(const float3& position);
	void Rotate(float pitch, float yaw);
	void SetAspectRatio(unsigned int screen_width, unsigned int screen_height);
	void SetFOV(float deg);
	void Zoom(float deg_diff);
	void Orbit(float pitch, float yaw, float3 object_position);
	void SetPlaneDistances(const float near_distance, const float far_distance);

	void Controller();

	Frustum frustum;
	float4x4 proj, view;

	float aspect_ratio;
	float horizontal_fov;
	float near_distance;
	float far_distance;
	float3 look_position;
	bool lock_view;
	float lock_distance;
	float3 position;

public:
	void WindowResized(unsigned int screen_width, unsigned int screen_height);

	float4x4 getProjection() { return frustum.ProjectionMatrix(); };
	float4x4 getView() { return float4x4(frustum.ViewMatrix()); };
};

