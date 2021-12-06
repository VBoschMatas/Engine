#pragma once
#include "Globals.h"
#include <math.h>

#define MSTOSEC (1.0f/1000.0f)
#define FPS_60 16.66666666666667f
#define TIMERBUFFER 30

class Timer
{
public:
	Timer();
	~Timer();

	void StartTime();
	unsigned int ReadTime();
	unsigned int StopTime();

private:
	bool time_running;
	unsigned int time_passed;
	unsigned int starting_time;
};

class TimeManager
{
private:
	size_t current_index;
	unsigned int current_frame;
	float frame_times_ms[TIMERBUFFER];
	float delta_time;
	float fps;
	Timer timer;

public:
	TimeManager() : current_index(-1), current_frame(0), delta_time(0.0f), fps(0.0f), timer()
	{
		std::fill_n(frame_times_ms, TIMERBUFFER, 0.0f);
	};

	~TimeManager() {};

	void Start()
	{
		timer.StartTime();
	};

	void End(bool cap_60 = true)
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
	};

	float FPS() const
	{
		return fps;
	};

	float DeltaTime() const
	{
		return delta_time * MSTOSEC;
	};

	float DeltaTimeMs() const
	{
		return delta_time;
	};

private:
	float CalculateFPS()
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

		return 1.0f / (sum * MSTOSEC * (1/ TIMERBUFFER));
	}
};

extern TimeManager* Time;