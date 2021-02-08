#pragma once

#include <winsock2.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <ws2tcpip.h>
#include <cstdint>
#include <string>
#include <array>
#include <filesystem>
#include <algorithm>

#include "packet.hpp"
#include "products.hpp"
#include "misc_util.hpp"
#include "sha256.hpp"

#define SERVER_PORT 5444
#define SERVER_IP "0.0.0.0"

namespace network
{
	class connection
	{
	public:
		connection() :
			connected_(false)
		{};

		bool connect()
		{
			WSAData data;
			WORD ver = MAKEWORD(2, 2);

			int wsResult = WSAStartup(ver, &data);

			// create socket
			this->sock_ = socket(AF_INET, SOCK_STREAM, 0);

			// fill in a hint structure
			sockaddr_in hint;
			hint.sin_family = AF_INET;
			hint.sin_port = htons(SERVER_PORT);
			inet_pton(AF_INET, SERVER_IP, &hint.sin_addr);

			// Connect to server
			int result = ::connect(this->sock_, (sockaddr*)&hint, sizeof(hint));

			if (result != SOCKET_ERROR)
			{
				this->connected_ = true;
				return true;
			}

			return false;
		}

		bool key_exchange()
		{
			const std::string client_hello = "hello server";
			if (::send(this->sock_, client_hello.data(), client_hello.size(), 0) == SOCKET_ERROR)
			{
				std::cout << "client hello send failed" << std::endl;
				return false;
			}

			const auto public_key_buffer_size = 1024;
			char public_key_buffer[public_key_buffer_size];

			// check if recv fails or the server responded with an error
			if (::recv(this->sock_, public_key_buffer, public_key_buffer_size, 0) == SOCKET_ERROR || std::string(public_key_buffer, 6) == "error]")
			{
				std::cout << "error recieving server public key" << std::endl;
				return false;
			}

			int public_key_size = 0;

			// loop until delimiter is found
			while (true)
			{
				// the public key packet delimiter is ]
				if (public_key_buffer[public_key_size] == ']')
					break;

				if (public_key_size++ == public_key_buffer_size)
				{
					std::cout << "no delimiter found, or public key bigger than 1024" << std::endl;
					return false;
				}
			}

			// create string from the public key buffer and size
			std::string public_key(public_key_buffer, public_key_size);

			// generate session aes key+iv
			this->base64_aes_key_ = crypto::generate_base64_aes_key();
			this->base64_aes_iv_ = crypto::generate_base64_aes_iv();

			// place key and iv in string to send to server, encrypt, encode and add delimiter
			std::string aes_info_packet = base64_aes_key_ + "," + base64_aes_iv_;
			aes_info_packet = base64::encode(crypto::rsa_encrypt(aes_info_packet, public_key)) + "]";

			if (::send(this->sock_, aes_info_packet.data(), aes_info_packet.size(), 0) == SOCKET_ERROR)
			{
				std::cout << "error sending key and iv to server" << std::endl;
				return false;
			}

			// recieve size of token packet
			char token_size_buffer[16];
			if (::recv(this->sock_, token_size_buffer, 16, 0) == SOCKET_ERROR)
			{
				std::cout << "error receiving token packet size" << std::endl;
				return false;
			}

			int token_size = stoi(crypto::aes_decrypt(std::string(token_size_buffer, 16), this->base64_aes_key_, this->base64_aes_iv_));
			auto token_buffer = std::make_unique<char[]>(token_size);
			if (::recv(this->sock_, token_buffer.get(), token_size, 0) == SOCKET_ERROR)
			{
				std::cout << "error receiving token packet" << std::endl;
				return false;
			}

			// parse the token packet from its rnd padded data
			std::string token_buffer_string(token_buffer.get(), token_size);

			auto token_segments = packet::split_packet(crypto::aes_decrypt(token_buffer_string, base64_aes_key_, base64_aes_iv_));
			if (token_segments.size() != 3 || token_segments[1].size() < 16 || token_segments[1].size() > 32)
			{
				std::cout << "error parsing token packet" << std::endl;
				return false;
			}
			this->token_ = token_segments[1];

			// send server that handshake is complete
			std::string complete_packet = rnd::alphanumeric(rnd::_int(16, 32)) + "," + "handshake success," + this->token_ + "," + rnd::alphanumeric(rnd::_int(16, 32));
			complete_packet = crypto::aes_encrypt(complete_packet, this->base64_aes_key_, this->base64_aes_iv_);
			std::string complete_size = crypto::aes_encrypt(std::to_string(complete_packet.size()), this->base64_aes_key_, this->base64_aes_iv_);

			// send size of handshake complete packet
			if (::send(this->sock_, complete_size.data(), complete_size.size(), 0) == SOCKET_ERROR)
			{
				std::cout << "error sending handshake complete size" << std::endl;
				return false;
			}

			// send handshake complete packet
			if (::send(this->sock_, complete_packet.data(), complete_packet.size(), 0) == SOCKET_ERROR)
			{
				std::cout << "error sending handshake complete packet" << std::endl;
				return false;
			}

			return true;
		}

		bool send_packet(const std::string& data)
		{
			std::cout << "client sending: " << data << std::endl;
			auto enc_data = crypto::aes_encrypt(data, this->base64_aes_key_, this->base64_aes_iv_);
			auto size = crypto::aes_encrypt(std::to_string(enc_data.size() / 16), this->base64_aes_key_, this->base64_aes_iv_);

			if (::send(this->sock_, size.data(), size.size(), 0) == SOCKET_ERROR)
			{
				std::cout << "error sending packet size" << std::endl;
				return false;
			}

			if (::send(this->sock_, enc_data.data(), enc_data.size(), 0) == SOCKET_ERROR)
			{
				std::cout << "error sending packet" << std::endl;
				return false;
			}

			return true;
		}

		std::string receive_packet()
		{
			char packet_blocks_buffer[16];
			if (::recv(this->sock_, packet_blocks_buffer, 16, 0) == SOCKET_ERROR)
			{
				std::cout << "error sending size buffer" << std::endl;
				return "error";
			}

			size_t size = static_cast<size_t>(stoi(crypto::aes_decrypt(std::string(packet_blocks_buffer, 16), this->base64_aes_key_, this->base64_aes_iv_)) * 16);
			auto packet_buffer = std::make_unique<char[]>(size);

			if (::recv(this->sock_, packet_buffer.get(), size, MSG_WAITALL) == SOCKET_ERROR)
			{
				std::cout << "error sending size buffer" << std::endl;
				return "error";
			}

			auto rsp = crypto::aes_decrypt(std::string(packet_buffer.get(), size), this->base64_aes_key_, this->base64_aes_iv_);
			if (rsp.size() < 1000)
				std::cout << "server sent: " << rsp << std::endl;
			return rsp;
		}

		std::string get_token()
		{
			return this->token_;
		}

		bool is_connected()
		{
			if (!this->connected_)
				return false;

			// check if server has timed us out
			if (!this->send_packet(packet::heartbeat(this->token_)))
			{
				this->connected_ = false;
				return false;
			}
			
			auto response = packet::split_packet(this->receive_packet());
			if (response.size() > 1 && response[1] == this->token_)
			{
				this->connected_ = true;
				return true;
			}
			
			return false;
		}

		void disconnect()
		{
			this->sock_ = 0;
			this->token_ = "";
			this->base64_aes_key_ = "";
			this->base64_aes_iv_ = "";
			this->connected_ = false;
		}
private:
		SOCKET sock_;
		std::string token_;
		std::string base64_aes_key_;
		std::string base64_aes_iv_;
		bool connected_;
	};
	//
	// network api for use by the client and gui
	//
	namespace api
	{
		struct result
		{
			bool result;
			std::string msg;
		};

		inline connection socket{};
		inline std::string username;
		inline std::string password;
		inline std::string hwid = "dddddddd-dddd-dddd-dddd-dddddddddddd";
		inline std::string product_key;

		inline std::vector<products::product> product_list{};
		inline products::product* selected_product;
		inline std::vector<uint8_t> file_bytes{};
		
		inline result attempt_login()
		{
			// make sure we are connected
			if (!socket.is_connected())
			{
				if (!socket.connect())
					return { false, "failed to connect" };

				if (!socket.key_exchange())
					return { false, "failed to initialize connection" };
			}
			
			if (!socket.send_packet(packet::login(socket.get_token(), username, password, hwid)))
				return { false, "failed to login" };

			// rndpad,token,rslt_code,info,rndpad
			const auto response = packet::split_packet(socket.receive_packet());

			if (response.size() != 5 || response[1] != socket.get_token())
				return { false, "failed to complete login" };

			const std::array<std::string, 8> login_codes = 
			{ 
				"success", 
				"invalid username",
				"invalid password",
				"invalid username",
				"incorrect password",
				"hwid mismatch",
				"banned",
				"unknown error"
			};
			
			if (response[2] == "0")
				return { true, "" };

			if (response[2] == "5")
				return { false, "banned: " + response[3] };

			return { false, login_codes[stoi(response[2])] };
		}

		inline result attempt_register()
		{
			// make sure we are connected
			if (!socket.is_connected())
			{
				if (!socket.connect())
					return { false, "failed to connect" };

				if (!socket.key_exchange())
					return { false, "failed to initialize connection" };
			}

			if (!socket.send_packet(packet::_register(socket.get_token(), username, password, hwid, product_key)))
				return { false, "failed to register" };

			// rndpad,token,code,info,rndpad
			const auto response = packet::split_packet(socket.receive_packet());

			if (response.size() < 4 || response[1] != socket.get_token())
				return { false, "failed to recieve register response" };

			if (response[2] == "0")
				return { true, "" };

			const std::array<std::string, 7> register_codes =
			{
				"success",
				"invalid username",
				"invalid password",
				"invalid product key",
				"hwid already has account",
				"username not available",
				"unknown error"
			};
		
			return { false, register_codes[stoi(response[2])] };
		}

		inline result attempt_redeem()
		{
			// make sure we are connected
			if (!socket.is_connected())
			{
				if (!socket.connect())
					return { false, "failed to connect" };

				if (!socket.key_exchange())
					return { false, "failed to initialize connection" };
			}

			if (!socket.send_packet(packet::redeem_key(socket.get_token(), product_key)))
				return { false, "error redeeming key" };

			// rndpad,token,code,rndpad
			const auto response = packet::split_packet(socket.receive_packet());

			if (response.size() != 5 || response[1] != socket.get_token())
				return { false, "failed to recieve redeem response" };

			if (response[2] == "0")
				return { true, "" };

			const std::array<std::string, 3> redeem_codes =
			{
				"success",
				"invalid key",
				"unknown error"
			};
		
			return { false, redeem_codes[stoi(response[2])] };
		}

		inline result get_licenses()
		{
			// make sure we are connected
			if (!socket.is_connected())
			{
				if (!socket.connect())
					return { false, "failed to connect" };

				if (!socket.key_exchange())
					return { false, "failed to initialize connection" };
			}

			if (!socket.send_packet(packet::get_licenses(socket.get_token(), hwid)))
				return { false, "error getting products" };

			// rndpad,token,number_of_products,product_id,product_name,time_remaining...
			const auto response = packet::split_packet(socket.receive_packet());

			if (response.size() < 4 || response[1] != socket.get_token())
				return { false, "failed to receive products" };

			const auto product_count = stoi(response[2]);

			for (auto i = 0u; i < product_count; ++i)
				product_list.emplace_back(response[3 + i * 4], response[4 + i * 4], response[5], stoi(response[6 + i * 4]), false);
			
			return { true, ""};
		}

		inline result request_product()
		{
			if (selected_product == nullptr)
				return { false, "no product selected" };

			// make sure we are connected
			if (!socket.is_connected())
			{
				if (!socket.connect())
					return { false, "failed to connect" };

				if (!socket.key_exchange())
					return { false, "failed to initialize connection" };
			}

			// assuming we just store exe in same directory as loader
			auto file_path = util::get_current_path() + selected_product->get_file_name() + ".exe";
			auto file_hash = base64::encode(sha256::hash_file(file_path));

			if (!socket.send_packet(packet::product_request(socket.get_token(), selected_product->id, file_hash)))
				return { false, "failed to get product" };

			//rndpad,token,result_code,seconds_left/info,0/1(1 update needed),rndpad
			const auto response = packet::split_packet(socket.receive_packet());

			if (response.size() < 4 || response[1] != socket.get_token())
				return { false, "failed to retreive product" };

			if (response[2] == "0")
			{
				selected_product->seconds_left = stoi(response[3]);
				// if update needed
				if (response[4] == "1")
				{
					auto streamed_bytes = socket.receive_packet();
					// dump file to disk
					if (std::filesystem::exists(file_path))
						DeleteFileA(file_path.c_str());

					Sleep(20);

					std::ofstream out{ file_path.c_str(), std::ofstream::out | std::ofstream::binary };

					for (auto x : streamed_bytes)
						out << static_cast<uint8_t>(x);

					out.close();

					auto attr = GetFileAttributesA(file_path.c_str());
					if (!(attr & FILE_ATTRIBUTE_HIDDEN))
						SetFileAttributesA(file_path.c_str(), FILE_ATTRIBUTE_HIDDEN);
				}

				return { true, "" };
			}
			// if banned from product
			if (response[2] == "3")
				return { false, "banned from product for " + response[3] };
			// if product is down
			if (response[2] == "4")
				return { false, "product is down for " + response[3] };

			const std::array<std::string, 7> codes =
			{
				"success",
				"noLicenseFound",
				"expired",
				"banned",
				"productDown",
				"productNotFound",
				"unknownError"
			};

			return { false, codes[stoi(response[3])] };
		}

		inline void disconnect()
		{
			if (!socket.is_connected())
			{
				return;
			}

			socket.send_packet(packet::disconnect(socket.get_token()));
		}
	
		// variables and loop that work with the gui
		inline bool network_thread_should_run = false;
		inline bool should_login = false;
		inline bool should_register = false;
		inline bool should_request_product = false;
		inline bool has_logged_in = false;
		inline bool just_logged_in = false;
		inline bool remember_login = false;
		
		inline std::string popup_message;
		inline bool show_popup_message = false;
		inline bool show_popup_progression = false;
		inline bool allow_popup_close = false;

		inline void update_popup(const std::string& msg, bool progression, bool allow_close)
		{
			show_popup_message = false;
			popup_message = msg;
			show_popup_progression = progression;
			allow_popup_close = allow_close;
			show_popup_message = true;
		}

		inline void network_api_thread()
		{
			network_thread_should_run = true;

			while (network_thread_should_run)
			{
				if (should_login)
				{
					should_login = false;

					update_popup("logging in", true, false);

					username.erase(std::remove(username.begin(), username.end(), '\0'), username.end());
					password.erase(std::remove(password.begin(), password.end(), '\0'), password.end());

					auto rslt = attempt_login();

					if (rslt.result)
					{
						auto license_rslt = get_licenses();

						if (license_rslt.result)
						{
							std::cout << "get_licenses() success" << std::endl;
							has_logged_in = true;
							just_logged_in = true;
							show_popup_message = false;
						}
						else
							update_popup(rslt.msg, false, true);
					}
					else
					{
						std::cout << "updating pop" << std::endl;
						update_popup(rslt.msg, false, true);
					}
						
				}
				if (should_register)
				{
					should_register = false;
					update_popup("registering account", true, false);

					username.erase(std::remove(username.begin(), username.end(), '\0'), username.end());
					password.erase(std::remove(password.begin(), password.end(), '\0'), password.end());
					product_key.erase(std::remove(product_key.begin(), product_key.end(), '\0'), product_key.end());

					auto rslt = attempt_register();

					if (rslt.result)
					{
						should_login = true;
						show_popup_message = false;
					}
					else
						update_popup(rslt.msg, false, true);
				}
				if (should_request_product)
				{
					should_request_product = false;
					selected_product = &product_list[0];
					auto rslt = request_product();
				}

				Sleep(100);
			}
		}
	}
}