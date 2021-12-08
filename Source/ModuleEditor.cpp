#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include <direct.h>
#include "Timer.h"
#include "GL/glew.h"
#include "SDL.h"
#include "IL/il.h"
#include "imGui/imgui.h"
#include "imGui/imgui_impl_sdl.h"
#include "imGui/imgui_impl_opengl3.h"
#include "windows.h"

#define PRINTFREQ 200

ModuleEditor::ModuleEditor()
{

}

ModuleEditor::~ModuleEditor()
{

}

bool ModuleEditor::Init()
{
	char* license_path = util::ConcatCStrings(App->GetWorkingDirectory(), "\\..\\LICENSE");
	util::ReadFile(license_file_path, &license_text);

	char buffer[MAX_PATH];
	_getcwd(buffer, MAX_PATH);

	FILE* file = nullptr;

	// In unix like systems and linux, r and rb are the same since they have single
	// character for endline, \n, but in windows there are multiple characters for
	// endline and additional b mode maps all those into \n.
	// r stands for read mode.
	fopen_s(&file, file_name, "rb");

	if (file)
	{
		static long file_offset = 0;
		size_t element_size_bytes = 1;
		fseek(file, file_offset, SEEK_END);
		int size = ftell(file);
		*data = (char*)malloc(size + 1);
		fseek(file, 0, SEEK_SET);
		fread(*data, element_size_bytes, size, file);
		(*data)[size] = 0;
		fclose(file);
	}

	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer->context);
	ImGui_ImplOpenGL3_Init();
	print_freq.StartTime();
	return true;
}

update_status ModuleEditor::PreUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();

	MainMenuBar();

	ConfigurationWindow();

	if(about_window)
		AboutWindow();
	if (console_window)
		ConsoleWindow();
	if (properties_window)
		PropertiesWindow();

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
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Tools"))
		{

			if (ImGui::MenuItem("Console", (const char*)0, console_window))
			{
				console_window = !console_window;
			}

			if (ImGui::MenuItem("Properties", (const char*)0, properties_window))
			{
				properties_window = !properties_window;
			}

			if (ImGui::MenuItem("Configuration", (const char*)0, configuration_window))
			{
				configuration_window = !configuration_window;
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
	ImGui::TextWrapped("This engine is being developed at the master degree \"Advanced programming for AAA Videogames\".\n");

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

	if (license_buffer == nullptr)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
		ImGui::TextWrapped("An error occured while fetching license.");
		ImGui::PopStyleColor();
	}
	else
	{
		ImGui::TextWrapped(license_buffer);
	}

	ImGui::End();
}

void ModuleEditor::ConsoleWindow()
{
	ImGui::Begin("Console", &console_window);

	console->ImGui();

	ImGui::End();
}

void ModuleEditor::PropertiesWindow()
{
	ImGui::Begin("Properties", &properties_window);



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