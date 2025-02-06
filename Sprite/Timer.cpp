#include "Timer.h"
#include <Windows.h>

Timer::Timer()
{
	uint64_t counts_per_second = 0;
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&counts_per_second));
	m_SecondsPerCount = 1.0f / static_cast<float>(counts_per_second);

	Reset();
}

void Timer::Start()
{
	uint64_t start_time = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&start_time);

	if (m_Stopped)
	{
		m_PausedTime += (start_time - m_StopTime);

		m_PreviousTime = start_time;
		m_StopTime = 0;
		m_Stopped = false;
	}
}

void Timer::Stop()
{
	if (!m_Stopped)
	{
		uint64_t current_time = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&current_time);

		m_StopTime = current_time;
		m_Stopped = true;
	}
}

void Timer::Reset()
{
	uint64_t current_time = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&current_time);

	m_BaseTime = current_time;
	m_PreviousTime = current_time;
	m_StopTime = 0;
	m_Stopped = false;

	Tick();
}

void Timer::Tick()
{
	if (m_Stopped)
	{
		m_DeltaTime = 0.0;
		return;
	}

	uint64_t current_time = 0;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&current_time));

	m_CurrentTime = current_time;

	// Time difference between this frame and the previous.
	m_DeltaTime = (m_CurrentTime - m_PreviousTime) * m_SecondsPerCount;

	// Prepare for next frame.
	m_PreviousTime = m_CurrentTime;

	if (m_DeltaTime < 0.0)
	{
		m_DeltaTime = 0.0;
	}
}

float Timer::DeltaTime() const
{
	return static_cast<float>(m_DeltaTime);
}

float Timer::TotalTime() const
{
	if (m_Stopped)
	{
		return static_cast<float>(((m_StopTime - m_PausedTime) - m_BaseTime) * m_SecondsPerCount);
	}
	else
	{
		return static_cast<float>(((m_CurrentTime - m_PausedTime) - m_BaseTime) * m_SecondsPerCount);
	}
}
