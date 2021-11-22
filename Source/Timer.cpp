#include "Timer.h"
#include "Application.h"
#include "SDL.h"

Timer::Timer()
{
	time_passed = 0;
	starting_time = 0;
	time_running = false;
}

Timer::~Timer()
{

}

void Timer::StartTime()
{
	time_passed = 0;
	starting_time = SDL_GetTicks();
	time_running = true;
}

unsigned int Timer::ReadTime()
{
	if (time_running) {
		time_passed = SDL_GetTicks() - starting_time;
	}
	return time_passed;
}

unsigned int Timer::StopTime()
{
	time_running = false;
	time_passed = SDL_GetTicks() - starting_time;
	return time_passed;
}