#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sha.h>
#include <vector>

namespace sha256
{
	inline std::string hash_file(const std::string& path)
	{
		std::vector<uint8_t> bytes{};

		std::ifstream in{ path.c_str(), std::ifstream::in | std::ifstream::binary };
		for (uint8_t byte; in >> std::noskipws >> byte;)
			bytes.push_back(byte);

		in.close();
		std::string digest;

		CryptoPP::SHA256 hash;
		hash.Update((CryptoPP::byte*)bytes.data(), bytes.size());
		digest.resize(hash.DigestSize());
		hash.Final((CryptoPP::byte*) &digest[0]);

		return digest;
	}

	inline std::string hash_file(const std::vector<uint8_t>& file_bytes)
	{
		std::string digest;

		CryptoPP::SHA256 hash;
		hash.Update((CryptoPP::byte*)file_bytes.data(), file_bytes.size());
		digest.resize(hash.DigestSize());
		hash.Final((CryptoPP::byte*) & digest[0]);

		return digest;
	}
}