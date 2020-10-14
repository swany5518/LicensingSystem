#pragma once

#include <string>
#include <iostream>
#include <rsa.h>
#include <aes.h>
#include <filters.h>
#include <files.h>
#include <hex.h>
#include <integer.h>
#include <modes.h>
#include <osrng.h>
#include <files.h>
#include <base64.h>
#include <memory>

#include "base64.hpp"

namespace crypto
{
	__forceinline std::string rsa_encrypt(const std::string& data, const std::string& base64_publickey)
	{
		CryptoPP::ByteQueue queue;
		CryptoPP::Base64Decoder decoder;

		// decode the key from base64
		decoder.Attach(new CryptoPP::Redirector(queue));
		decoder.Put((const CryptoPP::byte*)base64_publickey.data(), base64_publickey.length());
		decoder.MessageEnd();

		// initialize key and decode it from the queue
		CryptoPP::RSA::PublicKey key;
		key.BERDecode(queue);

		// set up rng, encryptor and string to hold cipher
		CryptoPP::AutoSeededRandomPool rng;
		CryptoPP::RSAES_OAEP_SHA_Encryptor encryptor(key);
		std::string cipher;

		// apply encryption
		CryptoPP::StringSource ss(data, true, new CryptoPP::PK_EncryptorFilter(rng, encryptor, new CryptoPP::StringSink(cipher)));

		return cipher;
	}

	__forceinline std::string rsa_decrypt(const std::string& data, const std::string& base64_privatekey)
	{
		// init ByteQueue and Base64 decoder
		CryptoPP::ByteQueue queue;
		CryptoPP::Base64Decoder decoder;

		// decode the key from base64
		decoder.Attach(new CryptoPP::Redirector(queue));
		decoder.Put((const CryptoPP::byte*)base64_privatekey.data(), base64_privatekey.length());
		decoder.MessageEnd();

		// create and decode the private key from the queue
		CryptoPP::RSA::PrivateKey key;
		key.BERDecode(queue);

		// set up rng, decryptor and string to hold decrypted
		CryptoPP::AutoSeededRandomPool rng;
		CryptoPP::RSAES_OAEP_SHA_Decryptor decryptor(key);
		std::string decrypted;

		// apply the decryption
		CryptoPP::StringSource ss(data, true, new CryptoPP::PK_DecryptorFilter(rng, decryptor, new CryptoPP::StringSink(decrypted)));

		return decrypted;
	}

	__forceinline std::string aes_encrypt(const std::string& data, const std::string& base64_key, const std::string& base64_iv)
	{
		// decode key and iv
		auto key = ::base64::decode(base64_key);
		auto iv = ::base64::decode(base64_iv);

		// init encryptor and set key and iv
		CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption encryptor;
		encryptor.SetKeyWithIV((CryptoPP::byte*)key.data(), key.size(), (CryptoPP::byte*)iv.data());

		// init string to hold encrypted
		std::string cipher;

		// apply encryption
		CryptoPP::StringSource ss(data, true, new CryptoPP::StreamTransformationFilter(encryptor, new CryptoPP::StringSink(cipher)));

		return cipher;
	}

	__forceinline std::string aes_decrypt(const std::string& data, const std::string& base64_key, const std::string& base64_iv)
	{
		// decode key and iv
		auto key = ::base64::decode(base64_key);
		auto iv = ::base64::decode(base64_iv);

		// init decryptor and set key and iv
		CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption decryptor;
		decryptor.SetKeyWithIV((CryptoPP::byte*)key.data(), key.size(), (CryptoPP::byte*)iv.data());
		
		// init string to hold decrypted
		std::string decrypted;

		// apply decryption
		CryptoPP::StringSource ss(data, true, new CryptoPP::StreamTransformationFilter(decryptor, new CryptoPP::StringSink(decrypted)));

		return decrypted;
	}

	__forceinline std::string generate_aes_key(int size = 16)
	{
		CryptoPP::AutoSeededRandomPool rnd;
		CryptoPP::SecByteBlock key(0x00, size);
		rnd.GenerateBlock(key, key.size());

		return std::string((char*)key.data(), key.size());
	}

	__forceinline std::string generate_aes_iv() 
	{
		CryptoPP::AutoSeededRandomPool rnd;
		CryptoPP::SecByteBlock iv(0x00, 16);
		rnd.GenerateBlock(iv, iv.size());

		return std::string((char*)iv.data(), iv.size());
	}
}