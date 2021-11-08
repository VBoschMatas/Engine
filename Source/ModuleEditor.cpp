#include "ModuleEditor.h"

ModuleEditor::ModuleEditor()
{

}

ModuleEditor::~ModuleEditor()
{

}

bool ModuleEditor::Init()
{
	ImGui::CreateContext();
	return true;
}

update_status ModuleEditor::Update()
{
	return UPDATE_CONTINUE;
}