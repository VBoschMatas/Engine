#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleWindow.h"
#include "ModuleEditorCamera.h"
#include "SDL/include/SDL.h"
#include "imgui_impl_sdl.h"

ModuleInput::ModuleInput()
{}

// Destructor
ModuleInput::~ModuleInput()
{}

// Called before render is available
bool ModuleInput::Init()
{
	DEBUG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		DEBUG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Called every draw update
update_status ModuleInput::PreUpdate()
{
    SDL_Event sdlEvent;

    while (SDL_PollEvent(&sdlEvent) != 0)
    {
        ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
        switch (sdlEvent.type)
        {
            case SDL_QUIT:
                return UPDATE_STOP;
            case SDL_WINDOWEVENT:
				if (sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED || sdlEvent.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					App->renderer->WindowResized(sdlEvent.window.data1, sdlEvent.window.data2);
					App->editorcamera->WindowResized(sdlEvent.window.data1, sdlEvent.window.data2);
				}
                break;
			case SDL_MOUSEMOTION:
				mouse_motion_x = sdlEvent.motion.xrel;
				mouse_motion_y = sdlEvent.motion.yrel;
				mouse_x = sdlEvent.motion.x;
				mouse_y = sdlEvent.motion.y;
				break;
			case SDL_MOUSEWHEEL:
				mouse_wheel = sdlEvent.wheel.y;
				break;
			case SDL_KEYUP:
				kybrd_button = sdlEvent.key.keysym.scancode;
				break;
			case SDL_DROPFILE:   // In case if dropped file
				dropped_filedir = sdlEvent.drop.file;
				App->scene->DroppedModel(dropped_filedir);
				SDL_free(dropped_filedir);    // Free dropped_filedir memory
				break;
        }
    }
    keyboard = SDL_GetKeyboardState(NULL);
	mouse_buttons = SDL_GetMouseState(NULL, NULL);

    return UPDATE_CONTINUE;
}

update_status ModuleInput::PostUpdate()
{
	mouse_motion_x = 0;
	mouse_motion_y = 0;
	mouse_wheel = 0;
	kybrd_button = NULL;
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	DEBUG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}