#pragma once

#include <chrono>

class timer
{
public:
	explicit timer()
		: start(std::chrono::high_resolution_clock::now().time_since_epoch())
	{}

	template <typename Duration>
	Duration elapsed_time()
	{
		return std::chrono::duration_cast<Duration>(std::chrono::high_resolution_clock::now().time_since_epoch() - start);
	}

	void reset()
	{
		start = std::chrono::high_resolution_clock::now().time_since_epoch();
	}

private:
	std::chrono::nanoseconds start;
};