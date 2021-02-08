#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <cassert>
#include <memory>
//#include <Windows.h>
#include <intrin.h>
#include <random>
#include <filesystem>
#include <tchar.h>

namespace hwid
{
	namespace hash
	{
		constexpr uint32_t Prime = 0x01000193;
		constexpr uint32_t Seed = 0x811C9DC5;
		// hash a single byte
		__forceinline uint32_t fnv1a(unsigned char oneByte, uint32_t hash = Seed)
		{
			return (oneByte ^ hash) * Prime;
		}

		// hash a 32 bit integer (four bytes)
		__forceinline uint32_t fnv1a(uint32_t fourBytes, uint32_t hash = Seed)
		{
			const unsigned char* ptr = (const unsigned char*)&fourBytes;
			hash = fnv1a(*ptr++, hash);
			hash = fnv1a(*ptr++, hash);
			hash = fnv1a(*ptr++, hash);
			return fnv1a(*ptr, hash);
		}

		__forceinline uint32_t fnv1a(uint64_t eightBytes, uint32_t hash = Seed)
		{
			const unsigned char* ptr = (const unsigned char*)&eightBytes;
			hash = fnv1a(*ptr++, hash);
			hash = fnv1a(*ptr++, hash);
			hash = fnv1a(*ptr++, hash);
			hash = fnv1a(*ptr++, hash);
			hash = fnv1a(*ptr++, hash);
			hash = fnv1a(*ptr++, hash);
			hash = fnv1a(*ptr++, hash);
			return fnv1a(*ptr, hash);
		}

		// hash a block of memory
		inline uint32_t fnv1a(const void* data, size_t numBytes, uint32_t hash = Seed)
		{
			assert(data);
			const unsigned char* ptr = (const unsigned char*)data;
			while (numBytes--)
				hash = fnv1a(*ptr++, hash);
			return hash;
		}

		// hash a c string
		inline uint32_t fnv1a(const char* text, uint32_t hash = Seed)
		{
			assert(text);
			while (*text)
				hash = fnv1a((unsigned char)*text++, hash);
			return hash;
		}
		// hash an std::string
		inline uint32_t fnv1a(const std::string& text, uint32_t hash = Seed)
		{
			return fnv1a(text.c_str(), text.length(), hash);
		}
	}

	namespace random
	{
		namespace detail
		{
			inline static std::mt19937& get_generator()
			{
				thread_local std::mt19937 generator{ std::random_device{}() };
				return generator;
			}
		}

		inline void set_seed(const unsigned seed)
		{
			detail::get_generator().seed(seed);
		}

		template <typename T>
		inline T get_integer(T min, T max)
		{
			std::uniform_int_distribution<T> distribution{ min, max };
			return distribution(detail::get_generator());
		}

		inline char get_char()
		{
			const auto value = get_integer(0u, 61u);

			if (value <= 9)
				return static_cast<char>(value + 48);

			if (value <= 35)
				return static_cast<char>(value + 55);

			return static_cast<char>(value + 61);
		}

		inline std::string get_string(const unsigned length)
		{
			const auto buffer = std::make_unique<char[]>(length);
			std::generate(buffer.get(), buffer.get() + length, []() -> char { return get_char(); });

			return { buffer.get(), buffer.get() + length };
		}

		inline std::string get_uuid(int seed)
		{
			random::set_seed(seed);
			return get_string(8) + "-" + get_string(4) + "-" + get_string(4) + "-" + get_string(4) + "-" + get_string(12);
		}

		inline std::string get_string(int length, int seed)
		{
			random::set_seed(seed);
			return get_string(length);
		}
	}

	inline std::string disk_serial()
	{
		// does not require admin privilege
		TCHAR volumeName[MAX_PATH + 1] = { 0 };
		TCHAR fileSystemName[MAX_PATH + 1] = { 0 };
		DWORD serialNumber = 0;
		DWORD maxComponentLen = 0;
		DWORD fileSystemFlags = 0;
		GetVolumeInformation(_T("C:\\"), volumeName, ARRAYSIZE(volumeName), &serialNumber, &maxComponentLen, &fileSystemFlags, fileSystemName, ARRAYSIZE(fileSystemName));

		return std::to_string(serialNumber);

		// requires admin privilege
		/*auto disk_identifier = []() -> std::string {
			using unique_handle = std::unique_ptr<std::remove_pointer_t<HANDLE>, decltype(&CloseHandle)>;

			const unique_handle disk_handle(CreateFileA(
				"\\\\.\\PhysicalDrive0",
				GENERIC_ALL,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				0,
				OPEN_EXISTING,
				0,
				0),
				&CloseHandle
			);

			if (!disk_handle)
				return { };

			STORAGE_PROPERTY_QUERY request_details{
				StorageDeviceProperty,
				PropertyStandardQuery,
				0
			};

			STORAGE_DESCRIPTOR_HEADER descriptor_header{ };

			if (!DeviceIoControl(disk_handle.get(), IOCTL_STORAGE_QUERY_PROPERTY, &request_details, sizeof(STORAGE_PROPERTY_QUERY), &descriptor_header, sizeof(STORAGE_DESCRIPTOR_HEADER), nullptr, 0))
				return { };

			const auto descriptor_size = descriptor_header.Size;
			std::unique_ptr<std::uint8_t[]> buffer{ new std::uint8_t[descriptor_size]{} };

			if (!DeviceIoControl(disk_handle.get(), IOCTL_STORAGE_QUERY_PROPERTY, &request_details, sizeof(STORAGE_PROPERTY_QUERY), buffer.get(), descriptor_size, nullptr, 0))
				return { };

			const auto device_descriptor = reinterpret_cast<PSTORAGE_DEVICE_DESCRIPTOR>(buffer.get());

			return std::string((const char*)buffer.get() + device_descriptor->SerialNumberOffset);
		};

		return disk_identifier();*/
	}

	inline std::string cpu_info()
	{
		int CPUInfo[4] = { -1 };
		unsigned   nExIds, i = 0;
		char CPUBrandString[0x40];
		// Get the information associated with each extended ID.
		__cpuid(CPUInfo, 0x80000000);
		nExIds = CPUInfo[0];
		for (i = 0x80000000; i <= nExIds; ++i)
		{
			__cpuid(CPUInfo, i);
			// Interpret CPU brand string
			if (i == 0x80000002)
				memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
			else if (i == 0x80000003)
				memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
			else if (i == 0x80000004)
				memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
		}

		return std::string(CPUBrandString);
	}

	inline std::string temp_path()
	{
		char buffer[256];
		GetTempPathA(sizeof(buffer), buffer);

		return std::string{ buffer };
	}

	inline std::string get_exe_path()
	{
		char result[MAX_PATH];
		auto filepath = std::string(result, GetModuleFileNameA(NULL, result, MAX_PATH));
		return filepath.substr(0, filepath.length() - std::distance(filepath.rbegin(), std::find(filepath.rbegin(), filepath.rend(), '\\')));
	}

	inline std::string username()
	{
		char buffer[256];
		GetTempPathA(sizeof(buffer), buffer);

		auto path = std::filesystem::path{ buffer };

		while (path.parent_path().stem() != "Users")
			path = path.parent_path();

		return path.stem().string();
	}

	inline std::string computer_name()
	{
		char* buf = nullptr;
		size_t sz = 0;
		_dupenv_s(&buf, &sz, "COMPUTERNAME");

		return std::string{ buf };
	}

	inline int file_date()
	{
		char buffer[256];
		::GetTempPathA(sizeof(buffer), buffer);

		auto path = std::filesystem::path{ buffer };

		while (path.has_relative_path())
			path = path.parent_path();

		path.append("\\Windows\\notepad.exe");

		return last_write_time(path).time_since_epoch().count();
	}

	inline std::string generate()
	{
		int seed = hash::fnv1a(disk_serial()) + hash::fnv1a(cpu_info()) ^ hash::fnv1a(username()) * hash::fnv1a(computer_name());
		return random::get_uuid(seed);
	}

	inline std::string generate_spoofer_safe()
	{
		int seed = hash::fnv1a(cpu_info()) + hash::fnv1a(username()) ^ hash::fnv1a(temp_path()) * hash::fnv1a(std::to_string(file_date()));
		return random::get_uuid(seed);
	}

	inline std::string get()
	{
		//return "{11111111-2222-3333-4444-555555555570}";
		int seed = hash::fnv1a(cpu_info()) + hash::fnv1a(username()) ^ hash::fnv1a(temp_path()) * hash::fnv1a(std::to_string(file_date()));
		return random::get_uuid(seed);
	}

	inline std::string generate_unique_file_name()
	{
		int seed = hash::fnv1a(temp_path()) * hash::fnv1a(username());
		return random::get_string(10, seed);
	}

	inline std::string generate_aes_key()
	{
		int seed = hash::fnv1a(temp_path()) + hash::fnv1a(username()) ^ hash::fnv1a(std::to_string(file_date()));
		return random::get_string(16, seed);
	}

	inline std::string generate_aes_iv()
	{
		int seed = hash::fnv1a(temp_path()) ^ hash::fnv1a(username()) & hash::fnv1a(std::to_string(file_date()));
		return random::get_string(16, seed);
	}
}