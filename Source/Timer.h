#pragma once
#include "Globals.h"

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

