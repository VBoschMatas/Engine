#include "ModuleEditorCamera.h"
#include "Application.h"
#include "Geometry/Frustum.h"
#include "GL/glew.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"

#define DEGTORAD pi / 180.0f
#define RADTODEG 180.0f / pi

#define MOVE_SPEED 5.0f
#define ROTATION_SPEED 1.0f
#define SHIFT_ACCELERATION 2

ModuleEditorCamera::ModuleEditorCamera()
{

}

ModuleEditorCamera::~ModuleEditorCamera()
{

}

bool ModuleEditorCamera::Init()
{
	lock_view = false;
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
	if (lock_view) LookAt(float3(0.0f, 0.0f, 0.0f));
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
	SetFOV(70.0f);
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


void ModuleEditorCamera::Rotate(float pitch, float yaw)
{
	Quat rotation;
	//Pitch
	if (pitch != 0.0f)
	{
		rotation = Quat::RotateAxisAngle(frustum.WorldRight(), pitch);
		frustum.SetUp(rotation.Mul(frustum.Up()).Normalized());
		frustum.SetFront(rotation.Mul(frustum.Front()).Normalized());
	}

	//Yaw
	if (yaw != 0.0f)
	{
		rotation = Quat::RotateY(yaw);
		frustum.SetFront(rotation.Mul(frustum.Front()).Normalized());
		frustum.SetUp(rotation.Mul(frustum.Up()).Normalized());
	}
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
	float temp_fov = (horizontal_fov * RADTODEG) + deg_diff;
	if (0.0f < temp_fov && temp_fov < 180.0f)
	{
		horizontal_fov = temp_fov * DEGTORAD;
		frustum.SetHorizontalFovAndAspectRatio(horizontal_fov, aspect_ratio);
	}
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
	float move_speed = MOVE_SPEED * Time->DeltaTime();
	float rotation_speed = ROTATION_SPEED * Time->DeltaTime();

	// Actions when Left Alt is pressed
	if (App->input->GetKey(SDL_SCANCODE_LALT))
	{
		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) || App->input->GetMouseButton(SDL_BUTTON_X1))
		{
			int mouse_x, mouse_y;
			App->input->GetMouseMovement(mouse_x, mouse_y);
			if (mouse_y != 0)
				Zoom((float)mouse_y * move_speed);
		}
	}
	else { // Use only mouse controlls if no special keys where used
		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) || App->input->GetMouseButton(SDL_BUTTON_X1))
		{
			float total_speed = move_speed;
			if (App->input->GetKey(SDL_SCANCODE_LSHIFT))
				total_speed *= SHIFT_ACCELERATION;
			//Movement with WASD
			if (App->input->GetKey(SDL_SCANCODE_W))
				position += frustum.Front() * total_speed;
			if (App->input->GetKey(SDL_SCANCODE_S))
				position -= frustum.Front() * total_speed;
			if (App->input->GetKey(SDL_SCANCODE_A))
				position -= frustum.WorldRight() * total_speed;
			if (App->input->GetKey(SDL_SCANCODE_D))
				position += frustum.WorldRight() * total_speed;
			if (App->input->GetKey(SDL_SCANCODE_Q))
				position += frustum.Up() * total_speed;
			if (App->input->GetKey(SDL_SCANCODE_E))
				position -= frustum.Up() * total_speed;

			// Mouse camera rotation
			int mouse_x, mouse_y;
			App->input->GetMouseMovement(mouse_x, mouse_y);
			if (mouse_x != 0 || mouse_y != 0)
				Rotate((float)mouse_y * -rotation_speed, (float)mouse_x * -rotation_speed);
		}
		else if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE))
		{
			int mouse_x, mouse_y;
			App->input->GetMouseMovement(mouse_x, mouse_y);
			//Movement with mouse if using middle button
			if (mouse_x > 0)
				position -= Abs((float)mouse_x) * frustum.WorldRight() * move_speed;
			if (mouse_x < 0)
				position += Abs((float)mouse_x) * frustum.WorldRight() * move_speed;
			if (mouse_y > 0)
				position += Abs((float)mouse_y) * frustum.Up() * move_speed;
			if (mouse_y < 0)
				position -= Abs((float)mouse_y) * frustum.Up() * move_speed;
		}
	}

	//Movement & rotation with arrow keys
	if (App->input->GetKey(SDL_SCANCODE_UP))
		position += frustum.Front() * move_speed;
	if (App->input->GetKey(SDL_SCANCODE_DOWN))
		position -= frustum.Front() * move_speed;
	if (App->input->GetKey(SDL_SCANCODE_LEFT))
		Rotate(0, rotation_speed);
	if (App->input->GetKey(SDL_SCANCODE_RIGHT))
		Rotate(0, -rotation_speed);

	if (App->input->GetMouseWheel() != 0)
		position += frustum.Front() * move_speed * (float)App->input->GetMouseWheel() * 2;

	frustum.SetPos(position);
}