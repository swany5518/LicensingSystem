#pragma once

#include <winsock2.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <ws2tcpip.h>
#include <cstdint>
#include <string>

#include "packet.hpp"

namespace network
{
#define SERVER_PORT 5444
#define SERVER_IP "127.0.0.1"

	inline SOCKET sock;
	inline std::string session_token;
	inline std::string session_aes_key;
	inline std::string session_aes_iv;
	inline bool connected = false;

	inline bool connect()
	{
		WSAData data;
		WORD ver = MAKEWORD(2, 2);

		int wsResult = WSAStartup(ver, &data);

		// create socket
		sock = socket(AF_INET, SOCK_STREAM, 0);

		// fill in a hint structure
		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(SERVER_PORT);
		inet_pton(AF_INET, SERVER_IP, &hint.sin_addr);

		// Connect to server
		int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));

		return connResult == 0;
	}

	inline bool key_exchange()
	{
		const std::string client_hello = "hello server";
		if (::send(sock, client_hello.data(), client_hello.size(), 0) == SOCKET_ERROR)
		{
			std::cout << "client hello send failed" << std::endl;
			return false;
		}

		const auto public_key_buffer_size = 1024;
		char public_key_buffer[public_key_buffer_size];

		// check if recv fails or the server responded with an error
		if (::recv(sock, public_key_buffer, public_key_buffer_size, 0) == SOCKET_ERROR || std::string(public_key_buffer, 6) == "error]")
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
		session_aes_key = crypto::generate_base64_aes_key();
		session_aes_iv = crypto::generate_base64_aes_iv();

		// place key and iv in string to send to server, encrypt, encode and add delimiter
		std::string aes_info_packet = session_aes_key + "," + session_aes_iv;
		aes_info_packet = base64::encode(crypto::rsa_encrypt(aes_info_packet, public_key)) + "]";

		if (::send(sock, aes_info_packet.data(), aes_info_packet.size(), 0) == SOCKET_ERROR)
		{
			std::cout << "error sending key and iv to server" << std::endl;
			return false;
		}
		
		// recieve size of token packet
		char token_size_buffer[16];
		if (::recv(sock, token_size_buffer, 16, 0) == SOCKET_ERROR)
		{
			std::cout << "error receiving token packet size" << std::endl;
			return false;
		}

		int token_size = stoi(crypto::aes_decrypt(std::string(token_size_buffer, 16), session_aes_key, session_aes_iv));
		auto token_buffer = std::make_unique<char[]>(token_size);
		if (::recv(sock, token_buffer.get(), token_size, 0) == SOCKET_ERROR)
		{
			std::cout << "error receiving token packet" << std::endl;
			return false;
		}
		
		// parse the token packet from its rnd padded data
		std::string token_buffer_string(token_buffer.get(), token_size);
		
		auto token_segments = split_packet(crypto::aes_decrypt(token_buffer_string, session_aes_key, session_aes_iv));
		if (token_segments.size() != 3 || token_segments[1].size() < 16 || token_segments[1].size() > 32)
		{
			std::cout << "error parsing token packet" << std::endl;
		}
		session_token = token_segments[1];

		// send server that handshake is complete
		std::string complete_packet = rnd::alphanumeric(rnd::_int(16, 32)) + "," + "handshake success," + session_token + "," + rnd::alphanumeric(rnd::_int(16, 32));
		complete_packet = crypto::aes_encrypt(complete_packet, session_aes_key, session_aes_iv);
		std::string complete_size = crypto::aes_encrypt(std::to_string(complete_packet.size()), session_aes_key, session_aes_iv);
		
		// send size of handshake complete packet
		if (::send(sock, complete_size.data(), complete_size.size(), 0) == SOCKET_ERROR)
		{
			std::cout << "error sending handshake complete size" << std::endl;
		}

		// send handshake complete packet
		if (::send(sock, complete_packet.data(), complete_packet.size(), 0) == SOCKET_ERROR)
		{
			std::cout << "error sending handshake complete packet" << std::endl;
		}
	}

	inline bool send_packet(std::string data)
	{
		std::cout << "sending: " << data << std::endl;
		std::string packet_blocks = std::to_string(data.size());
		while (packet_blocks.size() < 16)
			packet_blocks += " ";

		if (::send(sock, packet_blocks.c_str(), packet_blocks.size(), 0) == SOCKET_ERROR)
			return false;

		if (::send(sock, data.c_str(), data.size(), 0) == SOCKET_ERROR)
			return false;

		return true;
	}

	inline char* receive_packet()
	{
		char packet_size_buf[16];
		::recv(sock, packet_size_buf, 16, 0);

		auto str = std::string(packet_size_buf, 16);

		str.erase(std::remove_if(str.begin(), str.end(), std::isspace), str.end());

		int size = stoi(str);
		char buf[1024];
		::recv(sock, buf, size, 0);

		std::cout << "server response encoded size: " << size << std::endl;
		std::cout << base64::decode(std::string(buf, size)) << std::endl;

	}
}