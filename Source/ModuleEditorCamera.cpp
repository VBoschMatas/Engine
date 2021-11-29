#include "ModuleEditorCamera.h"
#include "Application.h"
#include "Geometry/Frustum.h"
#include "GL/glew.h"

#define DEGTORAD 57.29578

ModuleEditorCamera::ModuleEditorCamera()
{

}

ModuleEditorCamera::~ModuleEditorCamera()
{

}

bool ModuleEditorCamera::Init()
{
	//Transform params
	frustum.SetPerspective(math::pi / 4.0f, 2.f * atanf(tanf(frustum.VerticalFov() * 0.5f) * aspect));

	frustum.Pos = float3::zero;
	frustum.Front = -float3::unitZ;

	proj = frustum.ProjectionMatrix();

	return true;
}

update_status ModuleEditorCamera::PreUpdate()
{
	return UPDATE_CONTINUE;
}

update_status ModuleEditorCamera::Update()
{
	return UPDATE_CONTINUE;
}

update_status ModuleEditorCamera::PostUpdate()
{
	return UPDATE_CONTINUE;
}

bool ModuleEditorCamera::CleanUp()
{
	return true;
}

void ModuleEditorCamera::InitPerspectiveMatrix()
{
	Frustum frustum;
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetViewPlaneDistances(0.1f, 200.0f);
	frustum.SetHorizontalFovAndAspectRatio(DEGTORAD * 90.0f, 1.3f);

	frustum.SetPos(float3(0.0f, 1.0f, -2.0f));
	frustum.SetFront(float3::unitZ);
	frustum.SetUp(float3::unitY);

	float4x4 projectionGL = frustum.ProjectionMatrix().Transposed();

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(*projectionGL.v);
}

void ModuleEditorCamera::InitViewMatrix()
{
	
}