#pragma once
#include "Module.h"
#include "Globals.h"
#include "MathGeoLib.h"

class GameObject;

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

	float horizontal_fov;
	float aspect_ratio;
	float3 look_position;
	bool lock_view;
	float lock_distance;
	float3 position;

public:
	float field_of_view;
	float near_distance;
	float far_distance;

	void WindowResized(unsigned int screen_width, unsigned int screen_height);
	void ClickRaycast(float normalizedX, float normalizedY);
	void FitNewModel();

	void SetPosition(const float3& position);
	float3 GetPosition() { return position; };
	float getAspectRatio() { return aspect_ratio; };
	Frustum getFrustum() { return frustum; };
	void setCameraAs(Frustum _frustum);

	float4x4 getProjection() { return frustum.ProjectionMatrix(); };
	float4x4 getView() { return float4x4(frustum.ViewMatrix()); };
};

