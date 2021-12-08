#include <stdlib.h>
#include "Application.h"
#include "ModuleRender.h"
#include "Globals.h"
#include "Timer.h"

#include "SDL/include/SDL.h"
#pragma comment( lib, "SDL/lib/x64/SDL2.lib" )
#pragma comment( lib, "SDL/lib/x64/SDL2main.lib" )

enum main_states
{
	MAIN_CREATION,  
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

Application* App = NULL;
TimeManager* Time = NULL;
Console* console = NULL;

int main(int argc, char ** argv)
{
	int main_return = EXIT_FAILURE;
	main_states state = MAIN_CREATION;

	Time = new TimeManager();
	console = new Console();

	while (state != MAIN_EXIT)
	{
		switch (state)
		{
		case MAIN_CREATION:

			DEBUG("Application Creation --------------");
			App = new Application();
			state = MAIN_START;
			break;

		case MAIN_START:

			DEBUG("Application Init --------------");
			if (App->Init() == false)
			{
				DEBUG("Application Init exits with error -----");
				state = MAIN_EXIT;
			}
			else
			{
				state = MAIN_UPDATE;
				DEBUG("Application Update --------------");
			}

			break;

		case MAIN_UPDATE:
		{
			Time->Start();

			int update_return = App->Update();

			Time->End();

			if (update_return == UPDATE_ERROR)
			{
				DEBUG("Application Update exits with error -----");
				state = MAIN_EXIT;
			}

			if (update_return == UPDATE_STOP)
				state = MAIN_FINISH;
		}
			break;

		case MAIN_FINISH:

			DEBUG("Application CleanUp --------------");
			if (App->CleanUp() == false)
			{
				DEBUG("Application CleanUp exits with error -----");
			}
			else
				main_return = EXIT_SUCCESS;

			state = MAIN_EXIT;

			break;

		}

	}

	delete App;
	delete Time;
	delete console;
	DEBUG("Bye :)\n");
	return main_return;
}
