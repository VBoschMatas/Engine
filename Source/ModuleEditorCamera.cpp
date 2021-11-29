#include "ModuleEditorCamera.h"
#include "Application.h"
#include "Geometry/Frustum.h"
#include "GL/glew.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"

#define DEGTORAD pi / 180.0f

ModuleEditorCamera::ModuleEditorCamera()
{

}

ModuleEditorCamera::~ModuleEditorCamera()
{

}

bool ModuleEditorCamera::Init()
{
	InitPerspectiveMatrix();
	LookAt(float3(0.0f, 0.0f, 0.0f));
	return true;
}

update_status ModuleEditorCamera::PreUpdate()
{
	return UPDATE_CONTINUE;
}

update_status ModuleEditorCamera::Update()
{
	//LookAt(float3(0.0f, 0.0f, 0.0f));
	Controller();
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
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	auto screen_surface = App->window->screen_surface;
	SetAspectRatio(screen_surface->w, screen_surface->h);
	SetFOV(90.0f);
	SetPlaneDistances(0.1f, 100.0f);
	SetPosition(float3(0.0f, 1.0f, 10.0f));
	float3x3 identity = float3x3::identity;
	frustum.SetFront(identity.WorldZ());
	frustum.SetUp(identity.WorldY());

	proj = frustum.ProjectionMatrix();
}

void ModuleEditorCamera::SetPosition(const float3& _position)
{
	position = _position;
	frustum.SetPos(position);
}

void ModuleEditorCamera::LookAt(const float3& look_position)
{
	float3 direction = look_position - frustum.Pos();
	float3x3 look_dir = float3x3::LookAt(frustum.Front(), direction.Normalized(), frustum.Up(), float3::unitY);
	frustum.SetFront(look_dir.MulDir(frustum.Front()).Normalized());
	frustum.SetUp(look_dir.MulDir(frustum.Up()).Normalized());
	view = float4x4(frustum.ViewMatrix());
}


void ModuleEditorCamera::Rotate(float pich, float yaw)
{
	// TODO Rotation
}

void ModuleEditorCamera::SetAspectRatio(unsigned int screen_width, unsigned int screen_height)
{
	aspect_ratio = (float)screen_width / (float)screen_height;
	frustum.SetHorizontalFovAndAspectRatio(horizontal_fov, aspect_ratio);
}

void ModuleEditorCamera::SetFOV(float deg)
{
	horizontal_fov = deg * DEGTORAD;
	frustum.SetHorizontalFovAndAspectRatio(horizontal_fov, aspect_ratio);
}

void ModuleEditorCamera::Zoom(float deg_diff)
{
	//TODO Zoom
}

void ModuleEditorCamera::SetPlaneDistances(const float new_near_distance, const float new_far_distance)
{
	near_distance = new_near_distance;
	far_distance = new_far_distance;
	frustum.SetViewPlaneDistances(near_distance, far_distance);
}

void ModuleEditorCamera::WindowResized(unsigned int screen_width, unsigned int screen_height)
{
	SetAspectRatio(screen_width, screen_height);
}

void ModuleEditorCamera::InitViewMatrix()
{
	frustum.SetPos(float3(0.0, 1.0, 0.0));
}

void ModuleEditorCamera::Controller()
{
	static const float move_speed = 0.025f;

	float effective_speed = move_speed;

	if (App->input->GetKey(SDL_SCANCODE_W))
	{
		position += frustum.Front() * effective_speed;
	}
	if (App->input->GetKey(SDL_SCANCODE_S))
		position -= frustum.Front() * effective_speed;
	if (App->input->GetKey(SDL_SCANCODE_A))
		position -= frustum.WorldRight() * effective_speed;
	if (App->input->GetKey(SDL_SCANCODE_D))
		position += frustum.WorldRight() * effective_speed;
	if (App->input->GetKey(SDL_SCANCODE_Q))
		position += frustum.Up() * effective_speed;
	if (App->input->GetKey(SDL_SCANCODE_E))
		position -= frustum.Up() * effective_speed;

	frustum.SetPos(position);
}