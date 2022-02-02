#pragma once
#include "Module.h"
#include "Globals.h"
#include "ImGuizmo.h"
#include "GameObject.h"

enum class Window_Type : unsigned int
{
	Console,
	About,
	Config,
	MAX
};

enum MODE
{
	LOCAL,
	WORLD
};


class ModuleEditor : public Module
{
public:
	ModuleEditor();
	~ModuleEditor();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	bool isFocusScene() { return scene_selected; };
	bool isDebugDraw() { return debug_draw; };
	Game_State getGameState() { return game_state; }

	float2 scene_size = float2(0.0f, 0.0f);

private:

	//Windows
	bool console_window = true;
	bool about_window = false;
	bool configuration_window = false; 
	bool inspector_window = true;
	bool demo_window = true;
	bool showAppDockspace = true;

	bool scene_selected = false;
	bool draw_axis = false;
	bool draw_grid = false;
	bool debug_draw = false;

	Game_State game_state = Game_State::STOP;

	ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE gizmoMode;

	std::vector<float> ms;
	std::vector<float> fps;
	bool quit_engine = false;
	std::string license_text;

	ImVec2 currentWindowSize;
	Timer print_freq;

	void MainMenuBar(bool* p_open);
	void DockSpace();
	void SceneWindow();
	void SceneMenuBar();
	void AboutWindow();
	void ConsoleWindow();
	void ConfigurationWindow();
	void InspectorWindow();
	void HierarchyWindow();
	void ResourcesWindow();
	void VirtualMemory(float& total, float& consumed);
	void PhysicalMemory(float& total, float& consumed);
	void EditorCamera();
	void AmbientLight();

	float ToggleButtonColor(bool active);
	float ToggleTextColor(bool active);
	void HierarchyMenu();
	void GuizmosMenu();
	void PlayPauseMenu();
	void DebugDrawMenu();

	void SetStyle();
};
