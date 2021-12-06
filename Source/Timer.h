#pragma once
#include "Globals.h"
#include <math.h>
#include <algorithm>

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
public:
	TimeManager() : current_index(-1), current_frame(0), delta_time(0.0f), fps(0.0f), timer()
	{
		std::fill_n(frame_times_ms, TIMERBUFFER, 0.0f);
	};

	~TimeManager() {};

	void Start();

	void End(bool cap_60 = true);

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
private:
	size_t current_index;
	unsigned int current_frame;
	float frame_times_ms[TIMERBUFFER];
	float delta_time;
	float fps;
	Timer timer;

	float CalculateFPS();
};

extern TimeManager* Time;