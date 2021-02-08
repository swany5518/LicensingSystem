#include <iostream>
#include <cstdint>
#include <chrono>

#include "hwid.hpp"

namespace runtime_challenge
{
	struct exported_info
	{
		uint64_t challenge;
		uint64_t seconds_left;
	};

	inline uint64_t get_challenge(uint64_t time)
	{
		return hwid::hash::fnv1a(time) + hwid::hash::fnv1a((uint32_t)hwid::file_date()) ^ 0x7D93E;
	}

	inline uint64_t get_system_second()
	{
		return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	}

	inline uint64_t get_system_minute()
	{
		return std::chrono::duration_cast<std::chrono::minutes>(std::chrono::steady_clock::now().time_since_epoch()).count();
	}
}

namespace globals
{
	inline uint64_t startup_time;
}

extern "C"
{
	__declspec(dllexport) runtime_challenge::exported_info challenge_export;
}

void main_program_loop()
{
	while (runtime_challenge::get_system_second() - globals::startup_time <= challenge_export.seconds_left)
	{
		system("cls");
		std::cout << "you have " << challenge_export.seconds_left - (runtime_challenge::get_system_second() - globals::startup_time) << " seconds remaining!" << std::endl;
		Sleep(1000);
	}

	std::cout << "sorry, your license has expired" << std::endl;
}

int main()
{
	challenge_export.seconds_left = 100;
	globals::startup_time = runtime_challenge::get_system_second();

	for (auto i = 0u; i < 5; ++i)
	{
		if (runtime_challenge::get_challenge(globals::startup_time + i) == challenge_export.challenge || runtime_challenge::get_challenge(globals::startup_time - i) == challenge_export.challenge)
			main_program_loop();

		if (i == 4)
			std::cout << "you do not have access to this software" << std::endl;
	}
}