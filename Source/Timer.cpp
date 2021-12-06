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
//
// Time manager
//
void TimeManager::Start()
{
	timer.StartTime();
}

void TimeManager::End(bool cap_60)
{
	delta_time = timer.ReadTime();

	if (cap_60)
	{
		float delay_amount = std::max(0.0f, (FPS_60 - delta_time));

		delta_time += delay_amount;

		SDL_Delay(delay_amount);
	}

	current_index = (current_index + 1) % TIMERBUFFER;

	frame_times_ms[current_index] = delta_time;

	++current_frame;

	fps = CalculateFPS();
}

float TimeManager::CalculateFPS()
{
	float sum = 0.0f;
	for (size_t i = 0; i < TIMERBUFFER; ++i)
	{
		sum += frame_times_ms[i];
	}

	if (current_frame < TIMERBUFFER)
	{
		return 1.0f / (sum * MSTOSEC / (float)current_frame);
	}

	return 1.0f / (sum * MSTOSEC * (1 / TIMERBUFFER));
}