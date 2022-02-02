#include "Application.h"
#include "ModuleEditorCamera.h"
#include "ModuleScene.h"
#include "ComponentCamera.h"
#include "debugdraw.h"
#include "Math/float4x4.h"

#define DEGTORAD pi / 180.0f
#define RADTODEG 180.0f / pi

ComponentCamera::ComponentCamera()
{
	type = CompType::Camera;
	math::LCG math;
	uuid = math.Int();
	field_of_view = 70.0f;
	near_distance = 1.0f;
	far_distance = 20.0f;

	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	aspect_ratio = App->editorcamera->getAspectRatio();
	setFOV(field_of_view);
	frustum.SetViewPlaneDistances(near_distance, far_distance);
	frustum.SetPos(float3(0.0f, 0.0f, 0.0f));
	float3x3 identity = float3x3::identity;
	frustum.SetFront(identity.WorldZ());
	frustum.SetUp(identity.WorldY());
}

void ComponentCamera::Update(unsigned int program, const float3& position, const Quat& rotation, const float3& scale)
{
	aspect_ratio = App->editorcamera->getAspectRatio();
	setFOV(field_of_view);
	frustum.SetViewPlaneDistances(near_distance, far_distance);
	frustum.SetPos(position);
	frustum.SetFront(rotation.Mul(float3(0.0f, 0.0f, 1.0f)).Normalized());
	frustum.SetUp(rotation.Mul(float3(0.0f, 1.0f, 0.0f)).Normalized());
}

void ComponentCamera::DebugDraw()
{
	dd::cone(frustum.Pos(), frustum.Front(), dd::colors::White, 0.5f, 0.0f);
	dd::frustum(frustum.ViewProjMatrix().Inverted(), dd::colors::Red);
}

void ComponentCamera::printComponentInfo()
{
	static ImGuiTreeNodeFlags header_flags = ImGuiTreeNodeFlags_DefaultOpen;
	if (ImGui::CollapsingHeader("Camera", header_flags))
	{
		ImGui::DragFloat("Field of View", &field_of_view, 0.1f, 0.1f, 179.0f, "%.1f", 1.0f);
		ImGui::Text("\n");
		ImGui::DragFloat("Near Plane", &near_distance, 0.05f, 0.0f, FLT_MAX, "%.3f", 1.0f);
		ImGui::DragFloat("Far Plane", &far_distance, 0.05f, 0.0f, FLT_MAX, "%.3f", 1.0f);

		if (ImGui::Button("Set as Culling"))
		{
			App->scene->setCameraCulling(this);
		}
		ImGui::SameLine();
		if (ImGui::Button("Place Camera"))
		{
			App->editorcamera->setCameraAs(frustum);
		}
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Sets the editor camera parameters as this camera");
	}
}

void ComponentCamera::setFOV(float deg)
{
	float fov = deg * DEGTORAD;
	frustum.SetHorizontalFovAndAspectRatio(fov, aspect_ratio);
}

void ComponentCamera::Save(Archive* archive)
{
	archive->json["Component"] += {uuid, (int)type};
	Archive* go_archive = new Archive();
	go_archive->json = {
		{"Name", "Camera"},
		{"UUID", uuid},
		{"FOV", field_of_view},
		{"Near", near_distance},
		{"Far", far_distance}
	};
	go_archive->ToFile();
}