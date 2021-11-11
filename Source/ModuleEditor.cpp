#include "ModuleEditor.h"

ModuleEditor::ModuleEditor()
{

}

ModuleEditor::~ModuleEditor()
{

}

bool ModuleEditor::Init()
{
	//ImGui::CreateContext();
	return true;
}

update_status ModuleEditor::PreUpdate()
{
	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update()
{
	return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate()
{
	return UPDATE_CONTINUE;
}

bool ModuleEditor::CleanUp() {
	return true;
}