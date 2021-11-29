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

	Frustum frustum;
	float4x4 proj, view, model;

public:
	float4x4 getProjection() const;
	float4x4 getView() const;
};

