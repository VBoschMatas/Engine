#include "MicroTimer.h"
#include "SDL.h"

MicroTimer::MicroTimer()
{
	time_passed = 0;
	starting_time = 0;
	time_running = false;
	frequency = SDL_GetPerformanceFrequency();
}

MicroTimer::~MicroTimer()
{

}

void MicroTimer::StartTime()
{
	time_passed = 0;
	starting_time = SDL_GetPerformanceCounter();
	time_running = true;
}

float MicroTimer::ReadTime()
{
	if (time_running) {
		time_passed = (SDL_GetPerformanceCounter() - starting_time) / frequency;
	}
	return time_passed;
}

float MicroTimer::StopTime()
{
	time_running = false;
	time_passed = (SDL_GetPerformanceCounter() - starting_time) / frequency;
	return time_passed;
}