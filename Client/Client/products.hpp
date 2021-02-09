#include <string>
#include <cstdint>
#include <unordered_map>
#include <Windows.h>

#include <filesystem>
#include <fstream>
#include <cstdio>
#include <vector>

#include "hwid.h"

namespace products
{
	inline uint64_t time_loaded;

	inline void update_time_loaded()
	{
		time_loaded = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock().now().time_since_epoch()).count();
	}

	struct product
	{
		std::string id;
		std::string name;
		std::string status;
		uint64_t seconds_left;
		uint64_t time_loaded;
		bool selected;

		product(std::string id_, std::string name_, std::string status_, uint64_t seconds_left_, bool selected_)
		{
			id = id_;
			name = name_;
			status = status_;
			seconds_left = seconds_left_;
			selected = selected_;
			time_loaded = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock().now().time_since_epoch()).count();
		}

		uint64_t get_remaining_seconds() const
		{
			return (seconds_left - (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock().now().time_since_epoch()).count() - time_loaded));
		}

		std::string get_file_name() const
		{
			return name;
			//int seed = hwid::hash::fnv1a(id) + hwid::file_date() + hwid::hash::fnv1a(hwid::generate_aes_key());
			//return hwid::random::get_string(10, seed);
		}

		std::string get_product_time()
		{
			auto left = seconds_left - (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock().now().time_since_epoch()).count() - time_loaded);

			auto days_left = left / 86400;
			auto hours_left = (left / 3600) - (days_left * 24);

			return std::to_string(days_left) + "d " + std::to_string(hours_left) + "h";
		}

		bool time_left_color()
		{
			return (seconds_left - (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock().now().time_since_epoch()).count() - time_loaded)) / 86400;
		}

		bool is_launchable()
		{
			if (this->status == "down")
				return false;

			return true;
		}
	};

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

	namespace launcher
	{
		std::pair<bool, std::string> launch_product(const product& selected)
		{
			auto path = hwid::get_exe_path() + selected.get_file_name() + ".exe";
			auto hmod = LoadLibraryA(path.c_str());

			if (!hmod)
				return { false, "failed to get load module" };

			auto challenge_address = GetProcAddress(hmod, "challenge_export"); // maybe rename to throw off reverse engineers
			auto challenge_offset = reinterpret_cast<uintptr_t>(challenge_address) - reinterpret_cast<uintptr_t>(hmod);

			FreeLibrary(hmod);

			STARTUPINFOA si;
			PROCESS_INFORMATION pi;

			// set the size of the structures
			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);
			ZeroMemory(&pi, sizeof(pi));

			char _buffer[512];
			ZeroMemory(_buffer, sizeof _buffer);

			// start the program up
			if (!CreateProcessA(path.c_str(), NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi))
			{
				//std::cout << "CreateProcessA failed" << std::endl;
				return { false, "create process failed" };
			}

			// write challenge
			auto handle = OpenProcess(PROCESS_ALL_ACCESS, false, pi.dwProcessId);
			if (!handle)
			{
				//std::cout << "OpenProcess failed" << std::endl;
				return { false, "open process failed" };
			}

			auto challenge_hash = runtime_challenge::get_challenge(runtime_challenge::get_system_second());
			runtime_challenge::exported_info challenge{ challenge_hash, selected.get_remaining_seconds() };

			if (!WriteProcessMemory(handle, reinterpret_cast<LPVOID>(challenge_address), std::addressof(challenge), sizeof(challenge), nullptr))
			{
				//std::cout << "WPM failed" << std::endl;
				return { false, "wpm failed" };
			}

			CloseHandle(handle);

			// resume thread
			ResumeThread(pi.hThread);

			// close handles to process info
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);

			return { true, "success" };
		}
	}
}