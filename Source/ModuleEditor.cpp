#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleEditorCamera.h"
#include "ModuleDebugDraw.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "Model.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include <iostream>
#include <fstream>
#include <direct.h>
#include "Timer.h"
#include "GL/glew.h"
#include "SDL.h"
#include "IL/il.h"
#include "imGui/imgui.h"
#include "imGui/imgui_impl_sdl.h"
#include "imGui/imgui_impl_opengl3.h"
#include "ImGuizmo/ImGuizmo.h"
#include "windows.h"
#include "debugdraw.h"
#include "FontAwesome/IconsFontAwesome.h"
#include <vector>

#define PRINTFREQ 200
#define MAX_LIGHTS 8

ModuleEditor::ModuleEditor()
{

}

ModuleEditor::~ModuleEditor()
{

}

bool ModuleEditor::Init()
{

	//set inital Gizmo modes
	//transform_op = ImGuizmo::OPERATION::TRANSLATE;
	gizmoMode = ImGuizmo::MODE::WORLD;

	char buffer[MAX_PATH];
	_getcwd(buffer, MAX_PATH);

	std::string license_path(buffer);
	license_path = license_path.substr(0, license_path.find_last_of("\\"));
	license_path.append("\\LICENSE");
	std::string line;
	std::ifstream myfile(license_path);
	//myfile.open("example.txt");
	license_text = "";
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			license_text.append(line);
			license_text.append("\n");
		}
		myfile.close();
	}

	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer->context);
	ImGui_ImplOpenGL3_Init();
	print_freq.StartTime();

	SetStyle();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;


	return true;
}

update_status ModuleEditor::PreUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	MainMenuBar(&showAppDockspace);

	if (demo_window)
		ImGui::ShowDemoWindow();

	ResourcesWindow();
	ConfigurationWindow();
	EditorCamera();
	AmbientLight();

	if(about_window)
		AboutWindow();
	if (console_window)
		ConsoleWindow();
	if (inspector_window)
		InspectorWindow();

	SceneWindow();

	HierarchyWindow();

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update()
{
	//ImGui::ShowDemoWindow();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


	return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate()
{
	if (quit_engine) return UPDATE_STOP;

	return UPDATE_CONTINUE;
}

bool ModuleEditor::CleanUp() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	return true;
}

void ModuleEditor::MainMenuBar(bool* p_open)
{
	//Create a widow to contain all the docked elements
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::Begin("Menu", p_open, window_flags);

	//enable docking
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGui::DockSpace(ImGui::GetID("MyDockSpace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{

			if (ImGui::MenuItem("Save", (const char*)0))
			{
				App->scene->Save();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Tools"))
		{

			if (ImGui::MenuItem("Console", (const char*)0, console_window))
			{
				console_window = !console_window;
			}

			if (ImGui::MenuItem("Configuration", (const char*)0, configuration_window))
			{
				configuration_window = !configuration_window;
			}

			if (ImGui::MenuItem("Inspector", (const char*)0, inspector_window))
			{
				inspector_window = !inspector_window;
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{

			if (ImGui::MenuItem("GitHub"))
			{
				ShellExecute(NULL, "open", REPOSITORY_LINK, NULL, NULL, SW_SHOWNORMAL);
			}

			if (ImGui::MenuItem("About", (const char*)0, about_window))
			{
				about_window = !about_window;
			}

			if (ImGui::MenuItem("Quit"))
			{
				quit_engine = true;
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::End();
}

void ModuleEditor::AboutWindow()
{
	// Begin About Window:
	ImGui::Begin("About", &about_window);

	ImGui::AlignTextToFramePadding();
	ImGui::TextWrapped("Super Ultra Mega Awesome Engine");
	ImGui::Separator();
	ImGui::TextWrapped("This engine is being developed at the master degree \"Advanced programming for AAA Videogames\".");
	ImGui::TextWrapped("\n\nControlls:");
	ImGui::TextWrapped("UP/DOWN ARROW KEYS: move forward/backwards");
	ImGui::TextWrapped("LEFT/RIGHT ARROW KEYS: yaw rotation");
	ImGui::TextWrapped("MOUSE WHEEL: move forward/backwards");
	ImGui::TextWrapped("MOUSE WHEEL CLICK + MOUSE MOVEMENT: move camera up/down/left/right");
	ImGui::TextWrapped("F KEY: lock/unlock view on object");
	ImGui::TextWrapped("\nMOUSE RIGHT CLICK + MOUSE MOVEMENT: rotate camera");
	ImGui::TextWrapped("MOUSE RIGHT CLICK + Q/E: move camera up/down");
	ImGui::TextWrapped("MOUSE RIGHT CLICK + W/S: move camera forward/backwards");
	ImGui::TextWrapped("MOUSE RIGHT CLICK + A/D: move camera left/right");
	ImGui::TextWrapped("MOUSE RIGHT CLICK + SHIFT: double movement speed");
	ImGui::TextWrapped("\nALT BUTTON + MOUSE RIGHT CLICK: Change FOV");
	ImGui::TextWrapped("ALT BUTTON + MOUSE LEFT CLICK: Orbit object if view locked");

	ImGui::TextWrapped("\n");
	ImGui::TextWrapped("Author(s)");
	ImGui::Separator();
	ImGui::TextWrapped("Vicens Bosch Matas");

	ImGui::TextWrapped("\n");
	ImGui::TextWrapped("Libraries");
	ImGui::Separator();
	ImGui::TextWrapped(LIBRARIES);

	ImGui::TextWrapped("\n");
	ImGui::TextWrapped("License");
	ImGui::Separator();

	if (license_text == "")
	{
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
		ImGui::TextWrapped("Can't load the license");
		ImGui::PopStyleColor();
	}
	else
	{
		ImGui::TextWrapped(license_text.c_str());
	}

	ImGui::End();
}

void ModuleEditor::ConsoleWindow()
{
	ImGui::Begin("Console", &console_window);

	console->ImGui();

	ImGui::End();
}

void ModuleEditor::ConfigurationWindow()
{
	if (!configuration_window)
	{
		if (!fps.empty())
		{
			fps.clear();
		}

		if (!ms.empty())
		{
			ms.clear();
		}

		return;
	}

	if (fps.size() == TIMERBUFFER)
	{
		fps.erase(fps.begin());
	}
	if (ms.size() == TIMERBUFFER)
	{
		ms.erase(ms.begin());
	}

	if (print_freq.ReadTime() >= PRINTFREQ)
	{
		print_freq.StartTime();

		fps.push_back(Time->FPS());
		ms.push_back(Time->DeltaTimeMs());
	}
	
	ImGui::Begin("Configuration", &configuration_window);

	char title[25];

	sprintf_s(title, 25, "Framerate %.1f", fps.back());
	ImGui::PlotHistogram("##framerate", &fps.front(), fps.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));

	sprintf_s(title, 25, "Milliseconds %.1f", ms.back());
	ImGui::PlotHistogram("##milliseconds", &ms.front(), ms.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));

	ImGui::Separator();

	float tot_vir, con_vir, per_vir, tot_phy, con_phy, per_phy;

	VirtualMemory(tot_vir, con_vir);
	per_vir = (con_vir / tot_vir) * 100.0f;
	PhysicalMemory(tot_phy, con_phy);
	per_phy = (con_phy / tot_phy) * 100.0f;

	ImGui::TextWrapped("Virtual memory: %f / %f (%f\%)", con_vir, tot_vir, per_vir);
	ImGui::TextWrapped("Physical memory: %f / %f (%f\%)", con_phy, tot_phy, per_phy);

	char* vendor, *renderer, *opengl, *glsl, *glew;
	App->renderer->GetHardware(vendor, renderer, opengl, glsl, glew);
	if(ImGui::CollapsingHeader("Hardware"))
	{
		ImGui::TextWrapped("Vendor: %s", vendor);
		ImGui::TextWrapped("Renderer: %s", renderer);
		ImGui::TextWrapped("OpenGL version supported: %s", opengl);
		ImGui::TextWrapped("GLSL: %s", glsl);
	}
	SDL_version compiled;
	SDL_version linked;

	SDL_VERSION(&compiled);
	SDL_GetVersion(&linked);
	if (ImGui::CollapsingHeader("Software"))
	{
		ImGui::TextWrapped("SDL compiled version: %d.%d.%d", compiled.major, compiled.minor, compiled.patch);
		ImGui::TextWrapped("SDL linked version: %d.%d.%d", linked.major, linked.minor, linked.patch);
		ImGui::TextWrapped("OpenGL version: %s", glew);
		ImGui::TextWrapped("DevIL: %d", IL_VERSION);
	}

	ImGui::End();
}

void ModuleEditor::VirtualMemory(float& total, float& consumed)
{
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	total = memInfo.ullTotalPageFile;
	consumed = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
}

void ModuleEditor::PhysicalMemory(float& total, float& consumed)
{
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	total = memInfo.ullTotalPhys;
	consumed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
}

void ModuleEditor::InspectorWindow()
{
	ImGui::Begin("Inspector", &inspector_window);
	GameObject* inspected = App->scene->getSelectedGameObject();

	if (inspected != nullptr)
		inspected->printGameObjectInfo();

	ImGui::End();
}

void ModuleEditor::SceneWindow()
{
	ImGui::Begin("Scene");

	SceneMenuBar();

	if (ImGui::BeginChild("SceneCanvas", ImVec2(0, 0), true, ImGuiWindowFlags_NoMove))
	{
		scene_selected = ImGui::IsWindowFocused();
		if ((ImGui::IsMouseClicked(ImGuiMouseButton_Right) || ImGui::IsMouseClicked(ImGuiMouseButton_Middle)) && ImGui::IsWindowHovered())
			ImGui::SetWindowFocus();


		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered() && !App->input->GetKey(SDL_SCANCODE_LALT))
		{
			float normalizedX = -1.0 + 2.0 * (ImGui::GetMousePos().x - ImGui::GetWindowPos().x) / ImGui::GetWindowWidth();
			float normalizedY = 1.0 - 2.0 * (ImGui::GetMousePos().y - ImGui::GetWindowPos().y) / ImGui::GetWindowHeight();
			App->editorcamera->ClickRaycast(normalizedX, normalizedY);
		}

		float width = ImGui::GetWindowContentRegionWidth();
		float height = ImGui::GetContentRegionAvail().y;

		App->editorcamera->WindowResized(float(width), float(height));

		if (draw_axis == true)
		{
			dd::axisTriad(float4x4::identity, 0.5f, 5.0f);
		}
		if (draw_grid == true)
		{
			dd::xzSquareGrid(-500.0f, 500.0f, 0.0f, 2.0f, dd::colors::White);
		}

		ImVec2 windowSize = ImGui::GetWindowSize();
		ImGui::Image((ImTextureID)App->renderer->GetFBTexture(), windowSize, ImVec2(0, 1), ImVec2(1, 0));

		//DrawGuizmo(camera);
		//Gizmo stuff

		GameObject* selected = App->scene->getSelectedGameObject();
		if (selected) {
			ImGuizmo::SetOrthographic(false); //temp
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

			float4x4 cameraView = App->editorcamera->getView().Transposed();
			float4x4 cameraProjection = App->editorcamera->getProjection().Transposed();

			ComponentTransform* transformComponent = selected->Transform();
			float4x4 transformMatrix = transformComponent->getLocalTransform().Transposed();

			ImGuizmo::Manipulate(cameraView.ptr(), cameraProjection.ptr(), operation, gizmoMode, transformMatrix.ptr());

			if (ImGuizmo::IsUsing() || ImGuizmo::IsOver()) {
				transformComponent->setTransform(transformMatrix);
			}

		}

		

	}
	ImGui::EndChild();

	ImGui::End();
}

void ModuleEditor::SceneMenuBar()
{
	if (ImGui::BeginChild("SceneMenu", ImVec2(0, 20), true, ImGuiWindowFlags_NoMove))
	{
		ImGui::Columns(3, "SceneToolbar", false);
		GuizmosMenu();
		ImGui::NextColumn();
		PlayPauseMenu();
		ImGui::NextColumn();
		DebugDrawMenu();
		ImGui::NextColumn();
		ImGui::Columns(1);
	}
	ImGui::EndChild();
}

void ModuleEditor::GuizmosMenu()
{
	if (ImGui::IsKeyPressed(90))
		operation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(69))
		operation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(82)) // r Key
		operation = ImGuizmo::SCALE;

	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.65f, 0.6f, ToggleButtonColor(operation == ImGuizmo::TRANSLATE)));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.65f, 0.4f, ToggleButtonColor(operation == ImGuizmo::TRANSLATE)));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.65f, 0.2f, ToggleButtonColor(operation == ImGuizmo::TRANSLATE)));
	ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.65f, 0.0f, ToggleTextColor(operation == ImGuizmo::TRANSLATE)));
	if (ImGui::Button(ICON_FA_ARROWS))
	{
		operation = ImGuizmo::TRANSLATE;
	}
	ImGui::PopStyleColor(4);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Translate");

	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.65f, 0.6f, ToggleButtonColor(operation == ImGuizmo::ROTATE)));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.65f, 0.4f, ToggleButtonColor(operation == ImGuizmo::ROTATE)));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.65f, 0.2f, ToggleButtonColor(operation == ImGuizmo::ROTATE)));
	ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.65f, 0.0f, ToggleTextColor(operation == ImGuizmo::ROTATE)));
	if (ImGui::Button(ICON_FA_UNDO))
	{
		operation = ImGuizmo::ROTATE;
	}
	ImGui::PopStyleColor(4);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Rotate");

	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.65f, 0.6f, ToggleButtonColor(operation == ImGuizmo::SCALE)));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.65f, 0.4f, ToggleButtonColor(operation == ImGuizmo::SCALE)));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.65f, 0.2f, ToggleButtonColor(operation == ImGuizmo::SCALE)));
	ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.65f, 0.0f, ToggleTextColor(operation == ImGuizmo::SCALE)));
	if (ImGui::Button(ICON_FA_EXPAND))
	{
		operation = ImGuizmo::SCALE;
	}
	ImGui::PopStyleColor(4);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Scale");

	bool curr_culling_editor = App->scene->getCameraCulling() == nullptr;
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.65f, 0.6f, ToggleButtonColor(curr_culling_editor)));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.65f, 0.4f, ToggleButtonColor(curr_culling_editor)));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.65f, 0.2f, ToggleButtonColor(curr_culling_editor)));
	ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.65f, 0.0f, ToggleTextColor(curr_culling_editor)));
	ImGui::SameLine(0.0f, 120.0f);
	if (ImGui::Button(ICON_FA_CAMERA, ImVec2(30, 0)))
	{
		App->scene->setCameraCulling(nullptr);
	}
	ImGui::PopStyleColor(4);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Editor cam culling");
}
void ModuleEditor::PlayPauseMenu()
{
	ImGui::SameLine(ImGui::GetColumnWidth() / 2.0f - ((60.0f * 3.0f) / 2.0f));

	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.65f, 0.6f, ToggleButtonColor(game_state == Game_State::PLAY || game_state == Game_State::PAUSE)));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.65f, 0.4f, ToggleButtonColor(game_state == Game_State::PLAY || game_state == Game_State::PAUSE)));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.65f, 0.2f, ToggleButtonColor(game_state == Game_State::PLAY || game_state == Game_State::PAUSE)));
	ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.65f, 0.0f, ToggleTextColor(game_state == Game_State::PLAY || game_state == Game_State::PAUSE)));
	if (ImGui::Button(ICON_FA_PLAY, ImVec2(40, 0)))
	{
		game_state = Game_State::PLAY;
	}
	ImGui::PopStyleColor(4);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Play");

	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.65f, 0.6f, ToggleButtonColor(game_state == Game_State::PAUSE)));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.65f, 0.4f, ToggleButtonColor(game_state == Game_State::PAUSE)));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.65f, 0.2f, ToggleButtonColor(game_state == Game_State::PAUSE)));
	ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.65f, 0.0f, ToggleTextColor(game_state == Game_State::PAUSE)));
	if (ImGui::Button(ICON_FA_PAUSE, ImVec2(40, 0)))
	{
		if(game_state == Game_State::PLAY)
			game_state = Game_State::PAUSE;
		else if (game_state == Game_State::PAUSE)
			game_state = Game_State::PLAY;
	}
	ImGui::PopStyleColor(4);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Pause");

	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.65f, 0.6f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.65f, 0.4f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.65f, 0.2f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.65f, 0.0f, 1.0f));
	if (ImGui::Button(ICON_FA_STOP, ImVec2(40, 0)))
	{
		game_state = Game_State::STOP;
	}
	ImGui::PopStyleColor(4);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Stop");
}
void ModuleEditor::DebugDrawMenu()
{
	ImGui::Checkbox("Axis", &draw_axis);
	ImGui::SameLine();
	ImGui::Checkbox("Grid", &draw_grid);
	ImGui::SameLine();
	ImGui::Checkbox("Dbg Draw", &debug_draw);
	ImGui::SameLine();
	ImGui::Checkbox("Check Click", &App->dd->check_raycast);
}

void ModuleEditor::HierarchyWindow()
{
	ImGui::Begin("Hierarchy");

	HierarchyMenu();

	if ((ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Left)) && ImGui::IsWindowHovered())
	{
		App->scene->setSelectedGameObject(nullptr);
	}

	App->scene->printHierarchy();

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered())
	{
		if(ImGui::IsWindowFocused())
			ImGui::OpenPopup("Hierarchy Popup");
		ImGui::SetWindowFocus(); // Doing this at the second left click the popup will open
	}


	ImGui::End();
}

void ModuleEditor::HierarchyMenu()
{
	if (ImGui::BeginPopup("Hierarchy Popup"))
	{
		if (ImGui::BeginMenu("New"))
		{
			if (ImGui::MenuItem("Empty GameObject"))
			{
				GameObject* temp = App->scene->AddGameObject("EmptyObject", App->scene->getSelectedGameObject());
				if (App->scene->getSelectedGameObject() != nullptr)
					App->scene->getSelectedGameObject()->addChild(temp);
			}
			if (ImGui::MenuItem("Cube"))
			{
				GameObject* temp = App->scene->AddGameObject("Cube", App->scene->getSelectedGameObject(), GoType::Cube);
				if (App->scene->getSelectedGameObject() != nullptr)
					App->scene->getSelectedGameObject()->addChild(temp);
			}
			if (ImGui::MenuItem("Camera"))
			{
				GameObject* temp = App->scene->AddGameObject("Camera", App->scene->getSelectedGameObject(), GoType::Camera);
				if (App->scene->getSelectedGameObject() != nullptr)
					App->scene->getSelectedGameObject()->addChild(temp);
			}
			if (ImGui::BeginMenu("Light"))
			{
				if (ImGui::MenuItem("Directional"))
				{
					GameObject* temp = App->scene->AddGameObject("Directional Light", App->scene->getSelectedGameObject(), GoType::DirectionalLight);
					if (App->scene->getSelectedGameObject() != nullptr)
						App->scene->getSelectedGameObject()->addChild(temp);
				}
				if (ImGui::MenuItem("Point", "", false, App->scene->getNumPointLights() < MAX_LIGHTS))
				{
					GameObject* temp = App->scene->AddGameObject("Point Light", App->scene->getSelectedGameObject(), GoType::PointLight);
					if (App->scene->getSelectedGameObject() != nullptr)
						App->scene->getSelectedGameObject()->addChild(temp);
				}
				if (ImGui::MenuItem("Spot", "", false, App->scene->getNumSpotLights() < MAX_LIGHTS))
				{
					GameObject* temp = App->scene->AddGameObject("Spot Light", App->scene->getSelectedGameObject(), GoType::SpotLight);
					if (App->scene->getSelectedGameObject() != nullptr)
						App->scene->getSelectedGameObject()->addChild(temp);
				}

				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::Selectable("Remove", false))
		{
			if (App->scene->getSelectedGameObject() != nullptr)
			{
				App->scene->RemoveGameObject(App->scene->getSelectedGameObject());
			}
		}
		ImGui::EndPopup();
	}
}

void ModuleEditor::ResourcesWindow()
{
	ImGui::Begin("Resources");

	if (ImGui::CollapsingHeader("GameObjects"))
	{
		for (GameObject* go : App->scene->getGameObjects())
		{
			ImGui::TextWrapped(go->getName().c_str());
		}
	}

	if (ImGui::CollapsingHeader("Meshes"))
	{
		for (Mesh* mesh : App->scene->GetMeshes())
		{
			ImGui::TextWrapped(mesh->getName().c_str());
		}
	}

	if (ImGui::CollapsingHeader("Materials"))
	{
		for (Material* mat : App->scene->GetMaterials())
		{
			ImGui::TextWrapped(mat->name.c_str());
		}
	}

	if (ImGui::CollapsingHeader("Textures"))
	{
		std::map<unsigned int, Texture>* temp_textures = App->scene->GetTextures();
		for (auto& tex : *temp_textures)
		{
			ImGui::TextWrapped(tex.second.name.c_str());
		}
	}

	ImGui::End();
}

void ModuleEditor::EditorCamera()
{
	ImGui::Begin("Editor Camera");

	ImGui::DragFloat("Field of View", &App->editorcamera->field_of_view, 0.1f, 0.1f, 179.0f, "%.1f", 1.0f);
	ImGui::Text("\n");
	ImGui::DragFloat("Near Plane", &App->editorcamera->near_distance, 0.05f, 0.0f, FLT_MAX, "%.3f", 1.0f);
	ImGui::DragFloat("Far Plane", &App->editorcamera->far_distance, 0.05f, 0.0f, FLT_MAX, "%.3f", 1.0f);

	ImGui::End();
}

void ModuleEditor::AmbientLight()
{
	ImGui::Begin("Ambient Light");

	App->scene->AmbientLightInfo();

	ImGui::End();
}

void ModuleEditor::SetStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.ChildBorderSize = 0.0f;

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();

	// merge in icons from Font Awesome
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
	io.Fonts->AddFontFromFileTTF("fonts/FontAwesome/font.ttf", 11.0f, &icons_config, icons_ranges);
}

float ModuleEditor::ToggleButtonColor(bool active)
{
	if (active)
		return 0.4f;
	return 0.7f;
}

float ModuleEditor::ToggleTextColor(bool active)
{
	if (active)
		return 0.6f;
	return 1.0f;
}
