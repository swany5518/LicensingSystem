#pragma once

#include <string>
#include <base64.h>

namespace base64
{
	__forceinline std::string encode(const std::string& raw)
	{
		// init encoder and put raw data into it
		CryptoPP::Base64Encoder encoder;
		encoder.Put((CryptoPP::byte*)raw.data(), raw.size());
		encoder.MessageEnd();

		// put encoded data into a string
		std::string encoded;
		encoded.resize(encoder.MaxRetrievable());
		encoder.Get((CryptoPP::byte*)encoded.data(), encoded.size());

		return encoded;
	}

	__forceinline std::string encode(void* ptr, size_t size)
	{
		// init encoder and put raw data into it
		CryptoPP::Base64Encoder encoder;
		encoder.Put((CryptoPP::byte*)ptr, size);
		encoder.MessageEnd();

		// put encoded data into a string
		std::string encoded;
		encoded.resize(encoder.MaxRetrievable());
		encoder.Get((CryptoPP::byte*)encoded.data(), encoded.size());

		return encoded;
	}

	__forceinline std::string decode(const std::string& encoded)
	{
		// init decoder and put encoded data into it
		CryptoPP::Base64Decoder decoder;
		decoder.Put((CryptoPP::byte*)encoded.data(), encoded.size());
		decoder.MessageEnd();

		// put decoded data into a string
		std::string decoded;
		decoded.resize(decoder.MaxRetrievable());
		decoder.Get((CryptoPP::byte*) & decoded[0], decoded.size());

		return decoded;
	}

	__forceinline std::string decode(void* ptr, size_t size)
	{
		// init decoder and put encoded data into it
		CryptoPP::Base64Decoder decoder;
		decoder.Put((CryptoPP::byte*)ptr, size);
		decoder.MessageEnd();

		// put decoded data into a string
		std::string decoded;
		decoded.resize(decoder.MaxRetrievable());
		decoder.Get((CryptoPP::byte*) & decoded[0], decoded.size());

		return decoded;
	}
}