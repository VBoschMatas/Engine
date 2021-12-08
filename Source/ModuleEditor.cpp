#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "GL/glew.h"
#include "SDL.h"
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

	MainMenuBar();

	PerformanceWindow();

	if(about_window)
		AboutWindow();
	if (console_window)
		ConsoleWindow();

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
	if (quit_engine) return UPDATE_STOP;

	return UPDATE_CONTINUE;
}

bool ModuleEditor::CleanUp() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	return true;
}

void ModuleEditor::MainMenuBar()
{
	// Begin Drawing Main Menu Bar:
	if (ImGui::BeginMainMenuBar())
	{
		// Begin Drawing Tools Menu:
		if (ImGui::BeginMenu("Tools"))
		{

			if (ImGui::MenuItem("Console"))
			{
				// If user clicks on Console, trigger console window:
				console_window = true;
			}

			if (ImGui::MenuItem("Performance", (const char*) 0, performance_window))
			{
				performance_window = !performance_window;
			}

			// End Drawing Tools Menu:
			ImGui::EndMenu();
		}

		// Begin Drawing Help Menu:
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

			// End Drawing Help Menu:
			ImGui::EndMenu();
		}

		// End Drawing Main Menu Bar:
		ImGui::EndMainMenuBar();
	}
}

void ModuleEditor::AboutWindow()
{
	// Begin About Window:
	ImGui::Begin("About", &about_window);

	ImGui::AlignTextToFramePadding();
	ImGui::TextWrapped("Super Ultra Mega Awesome Engine");
	ImGui::Separator();
	ImGui::TextWrapped("This engine is being developed at the master degree \"Advanced programminfor AAA Videogames\".\n");

	ImGui::TextWrapped("\n");
	ImGui::TextWrapped("Repository");
	ImGui::Separator();
	ImGui::TextWrapped("Check my repository");
	if (ImGui::Button("Open Repository", ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
	{
		ShellExecute(NULL, "open", REPOSITORY_LINK, NULL, NULL, SW_SHOWNORMAL);
	}

	ImGui::TextWrapped("\n");
	ImGui::TextWrapped("Author(s)");
	ImGui::Separator();
	ImGui::TextWrapped("Vicenç Bosch Matas");

	ImGui::TextWrapped("\n");
	ImGui::TextWrapped("Libraries");
	ImGui::Separator();
	ImGui::TextWrapped(LIBRARIES);

	ImGui::TextWrapped("\n");
	ImGui::TextWrapped("License");
	ImGui::Separator();

	/*if (license_buffer == nullptr)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
		ImGui::TextWrapped("An error occured while fetching license.");
		ImGui::PopStyleColor();
	}
	else
	{
		ImGui::TextWrapped(license_buffer);
	}*/

	// End About Window:
	ImGui::End();
}

void ModuleEditor::PerformanceWindow()
{
	if (!performance_window)
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
	fps.push_back(Time->FPS());

	if (ms.size() == TIMERBUFFER)
	{
		ms.erase(ms.begin());
	}
	ms.push_back(Time->DeltaTimeMs());

	ImGui::Begin("Performance", &performance_window);

	char title[25];

	sprintf_s(title, 25, "Framerate %.1f", fps.back());
	ImGui::PlotHistogram("##framerate", &fps.front(), fps.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));

	sprintf_s(title, 25, "Milliseconds %.1f", ms.back());
	ImGui::PlotHistogram("##milliseconds", &ms.front(), ms.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));

	ImGui::End();
}

void ModuleEditor::ConsoleWindow()
{
	ImGui::Begin("Console", &console_window);

	console->ImGui();

	ImGui::End();
}