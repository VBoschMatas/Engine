#pragma once
class MicroTimer
{
public:
	MicroTimer();
	~MicroTimer();

	void StartTime();
	float ReadTime();
	float StopTime();

	float getFrequency() { return frequency; };
private:
	bool time_running;
	float time_passed;
	float starting_time;
	unsigned long long frequency;
};

