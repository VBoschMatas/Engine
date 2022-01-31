#include "ModuleEditorCamera.h"
#include "Application.h"
#include "Geometry/Frustum.h"
#include "GL/glew.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "Geometry/OBB.h"
#include "Geometry/Triangle.h"
#include "DebugDraw.h"
#include "ModuleDebugDraw.h"
#include "ModuleProgram.h"
#include <algorithm>
#include <functional>

#define DEGTORAD pi / 180.0f
#define RADTODEG 180.0f / pi

#define MOVE_SPEED 4.0f
#define ROTATION_SPEED 0.5f
#define SHIFT_ACCELERATION 2

ModuleEditorCamera::ModuleEditorCamera()
{
	lock_view = false;
	lock_distance = 0.0f;
	aspect_ratio = 0.0f;
	field_of_view = 0.0f;
	horizontal_fov = field_of_view * DEGTORAD;
	near_distance = 0.1f;
	far_distance = 1000.0f;
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
	horizontal_fov = field_of_view * DEGTORAD;
	SetFOV(field_of_view);
	frustum.SetViewPlaneDistances(near_distance, far_distance);
	Controller();

	if (lock_view) //For now all objects will be at 0, 0, 0. Once object selection is incorporated this will change
	{
		lock_distance = frustum.Pos().Distance(float3(0.0f, 0.0f, 0.0f));
		LookAt(float3(0.0f, 0.0f, 0.0f));
	}

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
	SetPlaneDistances(0.10f, 2000.0f);
	SetPosition(float3(0.0f, 50.0f, 100.0f));
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
		float3 temp_up = rotation.Mul(frustum.Up()).Normalized();
		if (RADTODEG * temp_up.AngleBetween(float3(0, 1, 0)) <= 90)
		{
			frustum.SetUp(rotation.Mul(frustum.Up()).Normalized());
			frustum.SetFront(rotation.Mul(frustum.Front()).Normalized());
		}
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
	field_of_view = deg;
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

void ModuleEditorCamera::Orbit(float pitch, float yaw, float3 object_position)
{
	float3 cam_position = frustum.Pos();
	SetPosition(object_position);
	Rotate(pitch, yaw);
	SetPosition(frustum.Front() * -lock_distance);
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
	if (!App->editor->isFocusScene())
		return;

	float move_speed = MOVE_SPEED * Time->DeltaTime();
	float rotation_speed = ROTATION_SPEED * Time->DeltaTime();

	// Actions when Left Alt is pressed
	if (App->input->GetKey(SDL_SCANCODE_LALT))
	{	// For some reason my laptop's mouse instead of giving "BUTTON_RIGHT" gives "BUTTON_X1" on right click
		// will be changed once I can use another mouse
		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) || App->input->GetMouseButton(SDL_BUTTON_X1))
		{
			int mouse_x, mouse_y;
			App->input->GetMouseMovement(mouse_x, mouse_y);
			if (mouse_y != 0)
				Zoom((float)mouse_y * move_speed);
		}
		// Only orbit if the model is focused
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) && lock_view)
		{
			int mouse_x, mouse_y;
			App->input->GetMouseMovement(mouse_x, mouse_y);
			if (mouse_x != 0 || mouse_y != 0) //Orbit now will orbit (0, 0, 0), once objects are selectable this will change
				Orbit((float)mouse_y * -rotation_speed, (float)mouse_x * -rotation_speed, float3(0.0f, 0.0f, 0.0f));
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
	{
		float wheel_speed = move_speed;
		//When the view is locked the camera will be slower the closest it is from the object
		if (lock_view)
			wheel_speed *= ((App->renderer->model->bounding_box.Distance(frustum.Pos()) + 0.5f)/3.0f);
		position += frustum.Front() * wheel_speed * (float)App->input->GetMouseWheel() * 2;
	}

	if (App->input->GetPressedKey(SDL_SCANCODE_F))
	{
		lock_view = !lock_view;
	}

	frustum.SetPos(position);
}

void ModuleEditorCamera::FitNewModel()
{
	float3 extreme_point = App->renderer->model->bounding_box.ExtremePoint(float3(1.0f, 1.0f, 1.0f));
	float extreme_dist = extreme_point.Distance(float3(0.0f, 0.0f, 0.0f)) / 10.0f;
	float new_distance = extreme_point.Distance(float3(0.0f, 0.0f, 0.0f));
	SetPosition(float3(1.0f, 1.0f, 1.0f) * extreme_dist);
	LookAt(float3(0.0f, 0.0f, 0.0f));
	while (!frustum.Contains(App->renderer->model->bounding_box) && new_distance < frustum.FarPlaneDistance())
	{
		SetPosition(float3(1.0f, 1.0f, 1.0f) * new_distance);
		LookAt(float3(0.0f, 0.0f, 0.0f));
		new_distance += extreme_dist;
	}
}

void ModuleEditorCamera::ClickRaycast(float normalizedX, float normalizedY)
{
	LineSegment ray = frustum.UnProjectLineSegment(normalizedX, normalizedY);
	App->dd->CheckRaycast(ray.a, ray.b);
	std::vector<GameObject*> hit_objects = {};
	App->scene->getQuadtree()->CollectIntersections(hit_objects, ray);
	GameObject* closest_go = nullptr;
	float closest_dist = FLT_MAX;
	for (GameObject* go : hit_objects)
	{
		LineSegment ray_local_space;
		ray_local_space = LineSegment(go->getInvertedTransform().TransformPos(ray.a), go->getInvertedTransform().TransformPos(ray.b));
		std::vector<math::Triangle> tris = go->getTriangles();
		float dist;
		float3 hit_point;
		for (math::Triangle const &tri : tris)
		{
			bool hit = ray_local_space.Intersects(tri, &dist, &hit_point);
			if (hit && dist < closest_dist)
			{
				closest_dist = dist;
				closest_go = go;
			}
		}
	}

	App->scene->setSelectedGameObject(closest_go);
}

void ModuleEditorCamera::setCameraAs(Frustum _frustum)
{
	frustum.SetUp(_frustum.Up());
	frustum.SetFront(_frustum.Front());
	SetPosition(_frustum.Pos());
	field_of_view = _frustum.HorizontalFov() * RADTODEG;
	near_distance = _frustum.NearPlaneDistance();
	far_distance = _frustum.FarPlaneDistance();
}