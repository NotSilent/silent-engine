#include "ProfillerTimer.h"

ProfillerTimer::ProfillerTimer()
{
	_start = std::chrono::high_resolution_clock::now();
}

float ProfillerTimer::end() const
{
	auto end = std::chrono::high_resolution_clock::now();

	std::chrono::duration<float> duration = end - _start;

    return duration.count();
}