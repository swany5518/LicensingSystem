#pragma once

#include <string>
#include <cstdint>
#include <sstream>

#include "rnd.hpp";
#include <functional>

namespace network
{
	enum class packet_type
	{
		initialize = 0,
		login,
		registration
	};

	struct packet
	{
		packet_type type_;
		std::string data_;

		packet(packet_type type, const std::string& data)
		{
			type_ = type;
			data_ = rnd::alphanumeric(rnd::_int(16, 32)) + ","
				+ std::to_string(static_cast<int>(type_)) + ","
				+ data + ","
				+ rnd::alphanumeric(rnd::_int(16, 32));
		}
	};

	namespace detail
	{
		inline std::vector<std::string> string_split(std::string str, char delim)
		{
			std::vector <std::string> tokens{};

			std::string token;
			std::istringstream token_stream{ str };

			while (std::getline(token_stream, token, delim))
				tokens.push_back(token);

			return tokens;
		}
	}

	inline static std::vector<std::string> split_packet(const std::string& packet)
	{
		auto split = detail::string_split(packet, ',');

		return split;
	}
}