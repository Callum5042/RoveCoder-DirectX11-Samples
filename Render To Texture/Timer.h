#pragma once

#include <cstdint>

// High resolution timer - https://docs.microsoft.com/en-us/windows/win32/sysinfo/acquiring-high-resolution-time-stamps
class Timer
{
public:
	Timer();
	~Timer() = default;

	// Starts the timer
	void Start();

	// Stops the timer
	void Stop();

	// Resets the timer
	void Reset();

	// Calculate the time since last called
	void Tick();

	// Gets the time between frame
	float DeltaTime() const;

	// Gets the total time since started
	float TotalTime() const;

private:
	float m_SecondsPerCount = 0.0;
	float m_DeltaTime = 0.0;

	int64_t m_BaseTime = 0;
	int64_t m_PausedTime = 0;
	int64_t m_StopTime = 0;
	int64_t m_PreviousTime = 0;
	int64_t m_CurrentTime = 0;

	bool m_Stopped = false;
};