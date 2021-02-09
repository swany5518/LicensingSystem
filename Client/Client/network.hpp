#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdint>
#include <string>
#include <array>
#include <filesystem>
#include <algorithm>
#include "iostream"

#include "packet.hpp"
#include "products.hpp"
#include "sha256.hpp"
#include "hwid.h"
#include "globals.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define SERVER_PORT 5444
#define SERVER_IP "165.227.219.1"

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
				//std::cout << "client hello send failed" << std::endl;
				return false;
			}

			const auto public_key_buffer_size = 1024;
			char public_key_buffer[public_key_buffer_size];

			// check if recv fails or the server responded with an error
			if (::recv(this->sock_, public_key_buffer, public_key_buffer_size, 0) == SOCKET_ERROR || std::string(public_key_buffer, 6) == "error]")
			{
				//std::cout << "error recieving server public key" << std::endl;
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
					//std::cout << "no delimiter found, or public key bigger than 1024" << std::endl;
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
				//std::cout << "error sending key and iv to server" << std::endl;
				return false;
			}

			// recieve size of token packet
			char token_size_buffer[16];
			if (::recv(this->sock_, token_size_buffer, 16, 0) == SOCKET_ERROR)
			{
				//std::cout << "error receiving token packet size" << std::endl;
				return false;
			}

			int token_size = stoi(crypto::aes_decrypt(std::string(token_size_buffer, 16), this->base64_aes_key_, this->base64_aes_iv_));
			auto token_buffer = std::make_unique<char[]>(token_size);
			if (::recv(this->sock_, token_buffer.get(), token_size, 0) == SOCKET_ERROR)
			{
				//std::cout << "error receiving token packet" << std::endl;
				return false;
			}

			// parse the token packet from its rnd padded data
			std::string token_buffer_string(token_buffer.get(), token_size);

			auto token_segments = packet::split_packet(crypto::aes_decrypt(token_buffer_string, base64_aes_key_, base64_aes_iv_));
			if (token_segments.size() != 3 || token_segments[1].size() < 16 || token_segments[1].size() > 32)
			{
				//std::cout << "error parsing token packet" << std::endl;
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
				//std::cout << "error sending handshake complete size" << std::endl;
				return false;
			}

			// send handshake complete packet
			if (::send(this->sock_, complete_packet.data(), complete_packet.size(), 0) == SOCKET_ERROR)
			{
				//std::cout << "error sending handshake complete packet" << std::endl;
				return false;
			}

			return true;
		}

		bool send_packet(const std::string& data)
		{
			//std::cout << "client sending: " << data << std::endl;
			auto enc_data = crypto::aes_encrypt(data, this->base64_aes_key_, this->base64_aes_iv_);
			auto size = crypto::aes_encrypt(std::to_string(enc_data.size() / 16), this->base64_aes_key_, this->base64_aes_iv_);

			if (::send(this->sock_, size.data(), size.size(), 0) == SOCKET_ERROR)
			{
				//std::cout << "error sending packet size" << std::endl;
				return false;
			}

			if (::send(this->sock_, enc_data.data(), enc_data.size(), 0) == SOCKET_ERROR)
			{
				//std::cout << "error sending packet" << std::endl;
				return false;
			}

			return true;
		}

		std::string receive_packet()
		{
			char packet_blocks_buffer[16];
			if (::recv(this->sock_, packet_blocks_buffer, 16, 0) == SOCKET_ERROR)
			{
				//std::cout << "error sending size buffer" << std::endl;
				return "error";
			}

			size_t size = static_cast<size_t>(stoi(crypto::aes_decrypt(std::string(packet_blocks_buffer, 16), this->base64_aes_key_, this->base64_aes_iv_)) * 16);
			auto packet_buffer = std::make_unique<char[]>(size);

			if (::recv(this->sock_, packet_buffer.get(), size, MSG_WAITALL) == SOCKET_ERROR)
			{
				//std::cout << "error sending size buffer" << std::endl;
				return "error";
			}

			auto rsp = crypto::aes_decrypt(std::string(packet_buffer.get(), size), this->base64_aes_key_, this->base64_aes_iv_);
			//if (rsp.size() < 1000)
				//std::cout << "server sent: " << rsp << std::endl;
			return rsp;
		}

		// meant for receiving files in fragments and updating progress
		std::string receive_packet_incremential(float* progress, bool print_progress = false)
		{
			char packet_blocks_buffer[16];
			if (::recv(this->sock_, packet_blocks_buffer, 16, 0) == SOCKET_ERROR)
			{
				//std::cout << "error sending size buffer" << std::endl;
				return "error";
			}
			size_t blocks = static_cast<size_t>(stoi(crypto::aes_decrypt(std::string(packet_blocks_buffer, 16), this->base64_aes_key_, this->base64_aes_iv_)));
			auto packet_buffer = std::make_unique<char[]>(blocks * 16);

			for (auto i = 0u; i < blocks; ++i)
			{
				if (::recv(this->sock_, &packet_buffer.get()[i * (size_t)16], 16, MSG_WAITALL) == SOCKET_ERROR)
					return "error";

				if (i % 16 == 0)
					*progress = static_cast<float>(i) / static_cast<float>(blocks);

				if (print_progress && i % 8000 == 0)
				{
					system("cls");
					printf("downloading update %.2f%%", static_cast<float>(i) / static_cast<float>(blocks) * 100.f);
				}
			}

			if (print_progress)
				std::cout << "\ndownload complete" << std::endl;

			auto rsp = std::string(packet_buffer.get(), blocks * 16);

			return crypto::aes_decrypt(rsp, this->base64_aes_key_, this->base64_aes_iv_);
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
		namespace detail
		{
			inline bool verify_key_format(const std::string& key)
			{
				//std::cout << key.size();
				if (key.size() != 36)
					return false;
				//std::cout << "key not 36" << std::endl;
				if (key[8] != '-' ||
					key[13] != '-' ||
					key[18] != '-' ||
					key[23] != '-')
					return false;

				return true;
			}

			inline bool verify_username_format(const std::string& username)
			{
				if (username.length() < 4 || username.length() > 24)
					return false;

				for (auto& c : username)
				{
					if (!isalnum(c) && (c != '-' && c != '_' && c != '.'))
						return false;
				}

				return true;
			}

			inline bool verify_password_format(const std::string& password)
			{
				if (password.length() < 4 || password.length() > 24)
					return false;

				for (auto& c : password)
				{
					if (!isalnum(c) && c == ',')
						return false;
				}

				return true;
			}
		}

		struct result
		{
			bool result;
			std::string msg;
		};

		inline connection socket{};
		inline std::string username;
		inline std::string password;
		inline std::string hwid = hwid::get();
		inline std::string product_key;

		inline std::vector<products::product> product_list{};
		inline products::product* selected_product;
		inline std::vector<uint8_t> file_bytes{};
		inline float file_progress = 0.f;

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

			if (!socket.send_packet(packet::login(socket.get_token(), username, password, hwid::get())))
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

			if (!socket.send_packet(packet::_register(socket.get_token(), username, password, hwid::get(), product_key)))
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

		// need to be logged in for these. 
		inline result attempt_redeem()
		{
			// make sure we are connected
			if (!socket.is_connected())
				return { false, "timed out" };

			if (!socket.send_packet(packet::redeem_key(socket.get_token(), product_key)))
				return { false, "error redeeming key" };

			// rndpad,token,code,rndpad
			const auto response = packet::split_packet(socket.receive_packet());

			if (response.size() < 4 || response[1] != socket.get_token())
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
				return { false, "timed out" };

			if (!socket.send_packet(packet::get_licenses(socket.get_token(), hwid::get())))
				return { false, "error getting products" };

			// rndpad,token,number_of_products,product_id,product_name,status,time_remaining...
			const auto response = packet::split_packet(socket.receive_packet());

			if (response.size() < 4 || response[1] != socket.get_token())
				return { false, "failed to receive products" };

			product_list.clear();
			const auto product_count = stoi(response[2]);

			for (auto i = 0u; i < product_count; ++i)
				product_list.emplace_back(response[3 + i * 4], response[4 + i * 4], response[5 + i * 4], stoi(response[6 + i * 4]), false);

			return { true, "" };
		}

		inline result request_product(bool* update_signal)
		{
			if (selected_product == nullptr)
				return { false, "no product selected" };

			// make sure we are connected
			if (!socket.is_connected())
				return { false, "timed out" };


			// assuming we just store exe in same directory as loader
			auto file_path = hwid::get_exe_path() + selected_product->get_file_name() + ".exe";
			auto file_hash = base64::encode(sha256::hash_file(file_path));

			if (!socket.send_packet(packet::product_request(socket.get_token(), selected_product->id, file_hash)))
				return { false, "failed to get product" };

			//rndpad,token,result_code,seconds_left/info,0/1(1 update needed),rndpad
			const auto response = packet::split_packet(socket.receive_packet());

			if (response.size() < 4 || response[1] != socket.get_token())
				return { false, "failed to retreive product" };

			if (response[2] == "0")
			{
				if (selected_product != nullptr)
					selected_product->seconds_left = stoi(response[3]);
				// if update needed
				if (response[4] == "1")
				{
					auto dump_to_disk = [&]()
					{
						*update_signal = true;
						auto streamed_bytes = socket.receive_packet_incremential(&file_progress);
						// dump file to disk
						if (std::filesystem::exists(file_path))
							DeleteFileA(file_path.c_str());

						Sleep(20);

						std::ofstream out{ file_path.c_str(), std::ofstream::out | std::ofstream::binary };

						for (auto x : streamed_bytes)
							out << x;

						out.close();

						auto attr = GetFileAttributesA(file_path.c_str());
						if (!(attr & FILE_ATTRIBUTE_HIDDEN))
							SetFileAttributesA(file_path.c_str(), FILE_ATTRIBUTE_HIDDEN);
					};

					dump_to_disk();
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

			return { false, codes[stoi(response[2])] };
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
		inline bool should_redeem = false;
		inline bool should_clear_key = false;
		inline bool has_logged_in = false;
		inline bool just_logged_in = false;
		inline bool just_timed_out = false;

		inline std::string popup_message;
		inline bool show_popup_message = false;
		inline bool show_popup_progression = false;
		inline bool show_popup_progress_bar = false;
		inline bool allow_popup_close = false;

		inline void update_popup(const std::string& msg, bool progression, bool allow_close, bool bar = false)
		{
			show_popup_message = false;
			popup_message = msg;
			show_popup_progression = progression;
			allow_popup_close = allow_close;
			show_popup_progress_bar = bar;
			show_popup_message = true;
		}

		inline void network_api_thread()
		{
			network_thread_should_run = true;

			while (!globals::should_exit)
			{
				if (should_login)
				{
					should_login = false;

					update_popup("logging in", true, false);

					username.erase(std::remove(username.begin(), username.end(), '\0'), username.end());
					password.erase(std::remove(password.begin(), password.end(), '\0'), password.end());

					if (!detail::verify_username_format(username))
						update_popup("invalid username", false, true);
					else if (!detail::verify_password_format(password))
						update_popup("invalid password", false, true);
					else
					{
						auto rslt = attempt_login();

						if (rslt.result)
						{
							auto license_rslt = get_licenses();

							if (license_rslt.result)
							{
								has_logged_in = true;
								just_logged_in = true;
								show_popup_message = false;
							}
							else
								update_popup(rslt.msg, false, true);
						}
						else
						{
							//std::cout << "updating pop") << std::endl;
							update_popup(rslt.msg, false, true);
						}
					}
				}
				if (should_register)
				{
					should_register = false;

					update_popup("registering account", true, false);

					username.erase(std::remove(username.begin(), username.end(), '\0'), username.end());
					password.erase(std::remove(password.begin(), password.end(), '\0'), password.end());
					product_key.erase(std::remove(product_key.begin(), product_key.end(), '\0'), product_key.end());

					if (!detail::verify_username_format(username))
						update_popup("invalid username", false, true);
					else if (!detail::verify_password_format(password))
						update_popup("invalid password", false, true);
					else if (!detail::verify_key_format(product_key))
						update_popup("invalid key", false, true);
					else
					{
						auto rslt = attempt_register();

						if (rslt.result)
						{
							should_clear_key = true;
							should_login = true;
							show_popup_message = false;
						}
						else
							update_popup(rslt.msg, false, true);
					}
				}
				if (should_request_product)
				{
					should_request_product = false;
					update_popup("loading product", true, false);

					auto rslt = request_product(&show_popup_progress_bar);
					if (rslt.result)
					{
						show_popup_message = false;
						std::pair<bool, std::string> pair_rsp;
						update_popup("starting product", true, false);

						pair_rsp = products::launcher::launch_product(*selected_product);

						if (pair_rsp.first)
							globals::should_exit = true;
						else
							update_popup("error launching product", false, true);
					}
					else if (!rslt.result && rslt.msg == "timed out")
					{
						// we have been timed out and need to log in again
						has_logged_in = false;
						just_timed_out = true;
						selected_product = nullptr;

						update_popup(rslt.msg, false, true);
					}
					else
					{
						update_popup(rslt.msg, false, true);
					}
				}
				if (should_redeem)
				{
					should_redeem = false;

					update_popup("redeeming key", true, false);

					auto rslt = attempt_redeem();

					if (rslt.result)
					{
						should_clear_key = true;

						auto license_rslt = get_licenses();

						if (license_rslt.result)
							update_popup("key redeemed", false, true);
						else
							update_popup("key redeemed, " + license_rslt.msg, false, true);
					}
					else if (!rslt.result && rslt.msg == "timed out")
					{
						// we have been timed out and need to log in again
						should_clear_key = true;
						has_logged_in = false;
						just_timed_out = true;
						selected_product = nullptr;

						update_popup(rslt.msg, false, true);
					}
					else
					{
						should_clear_key = true;
						update_popup(rslt.msg, false, true);
					}
				}

				Sleep(100);
			}
		}
	}
}