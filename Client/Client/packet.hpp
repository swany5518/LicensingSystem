#pragma once

#include <string>
#include <cstdint>
#include <sstream>

#include "rnd.hpp";
#include <functional>

namespace network
{
	namespace packet
	{
		enum class type
		{
			login = 0,
			_register,
			redeem_key,
			get_licenses,
			product_request,
			client_update
		};

		//
		// functions to get packets
		//
		inline std::string login(const std::string& token, const std::string& username, const std::string& password, const std::string& hwid)
		{
			// randompadding,sessiontoken,0,username,password,hwid,randomPadding
			return rnd::alphanumeric(rnd::_int(16, 32)) + ","
				+ token + ","
				+ std::to_string(static_cast<int>(type::login)) + ","
				+ username + ","
				+ password + ","
				+ hwid + ","
				+ rnd::alphanumeric(rnd::_int(16, 32));
		}

		inline std::string _register(const std::string& token, const std::string& username, const std::string& password, const std::string& hwid, const std::string& key)
		{
			// randompadding,sessiontoken,1,username,password,hwid,productKey,randomPadding
			return rnd::alphanumeric(rnd::_int(16, 32)) + ","
				+ token + ","
				+ std::to_string(static_cast<int>(type::_register)) + ","
				+ username + ","
				+ password + ","
				+ hwid + ","
				+ key + ","
				+ rnd::alphanumeric(rnd::_int(16, 32));
		}

		inline std::string redeem_key(const std::string& token, const std::string& key)
		{
			// randompadding,sessiontoken,2,key,randomPadding
			return rnd::alphanumeric(rnd::_int(16, 32)) + ","
				+ token + ","
				+ std::to_string(static_cast<int>(type::redeem_key)) + ","
				+ key + ","
				+ rnd::alphanumeric(rnd::_int(16, 32));

		}

		inline std::string get_licenses(const std::string& token, const std::string& hwid)
		{
			// randompadding,sessiontoken,3,hwid,randompadding
			return rnd::alphanumeric(rnd::_int(16, 32)) + ","
				+ token + ","
				+ std::to_string(static_cast<int>(type::get_licenses)) + ","
				+ hwid + ","
				+ rnd::alphanumeric(rnd::_int(16, 32));
		}

		inline std::string product_request(const std::string& token, const std::string& product_id, const std::string& file_hash)
		{
			// randompadding,sessiontoken,4,productID,fileHash,randomPadding
			return rnd::alphanumeric(rnd::_int(16, 32)) + ","
				+ token + ","
				+ std::to_string(static_cast<int>(type::product_request)) + ","
				+ product_id + ","
				+ file_hash + ","
				+ rnd::alphanumeric(rnd::_int(16, 32));
		}

		inline std::string client_update(const std::string& token, const std::string& hwid, const std::string& client_hash)
		{
			// randompadding,sessiontoken,5,hwid,clientHash,randomPadding
			return rnd::alphanumeric(rnd::_int(16, 32)) + ","
				+ token + ","
				+ std::to_string(static_cast<int>(type::client_update)) + ","
				+ hwid + ","
				+ client_hash + ","
				+ rnd::alphanumeric(rnd::_int(16, 32));
		}
	}
}