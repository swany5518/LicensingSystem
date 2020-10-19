#pragma once

#include <random>
#include <iostream>

namespace rnd
{
	static std::random_device device;
	static std::mt19937 generator(device());
	static std::uniform_int_distribution<int> distribution(1, 100);

	inline bool coinflip()
	{
		return distribution(generator) % 2 == 0;
	}

	inline int _int(int min, int max)
	{
		std::uniform_int_distribution<int> dist(min, max);

		return dist(generator);
	}

	inline int signed_int(int min, int max)
	{
		int rnd = _int(min, max);

		return coinflip() ? rnd : rnd * -1;
	}

	inline bool percentage(float percent)
	{
		return distribution(generator) <= percent;
	}

	inline char letter()
	{
		std::uniform_int_distribution<int> dist(65, 90);

		char c = static_cast<char>(dist(generator));

		if (coinflip())
			c = tolower(c);

		return c;
	}

	inline std::string _string(int length)
	{
		std::string temp = "";

		for (auto i = 0u; i < length; ++i)
			temp += letter();

		return temp;
	}

	inline std::wstring _wstring(int length)
	{
		std::wstring temp;

		for (auto i = 0u; i < length; ++i)
			temp += letter();

		return temp;
	}

	inline std::string alphanumeric(size_t length)
	{
		static auto& chrs = "0123456789"
			"abcdefghijklmnopqrstuvwxyz"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

		thread_local static std::mt19937 rg{ std::random_device{}() };
		thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);

		std::string s;

		s.reserve(length);

		while (length--)
			s += chrs[pick(rg)];

		return s;
	}
}