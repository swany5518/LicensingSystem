#pragma once

#include <string>
#include <cstdint>

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
}