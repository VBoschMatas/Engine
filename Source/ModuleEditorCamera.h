#pragma once
#include "Module.h"
#include "Globals.h"

class ModuleEditorCamera: public Module
{
public:
	ModuleEditorCamera();
	~ModuleEditorCamera();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

private:
	void InitPerspectiveMatrix();
	void InitViewMatrix();

};

