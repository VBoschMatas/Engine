#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "GL/glew.h"

#include "imGui/imgui.h"
#include "imGui/imgui_impl_sdl.h"
#include "imGui/imgui_impl_opengl3.h"

ModuleEditor::ModuleEditor()
{

}

ModuleEditor::~ModuleEditor()
{

}

bool ModuleEditor::Init()
{
	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer->context);
	ImGui_ImplOpenGL3_Init();
	return true;
}

update_status ModuleEditor::PreUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();

	MenuBar(menu_items);

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate()
{
	return UPDATE_CONTINUE;
}

bool ModuleEditor::CleanUp() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	return true;
}

void ModuleEditor::MenuBar(const char** items)
{
	ImGui::BeginMainMenuBar();
	for (int i = 0; i < MENU_OPTIONS; ++i)
		MenuOption(items[i]);
	ImGui::EndMainMenuBar();
}

void ModuleEditor::MenuOption(const char* option)
{
	ImGui::Text(option);
}