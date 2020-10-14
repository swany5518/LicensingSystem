#include "crypto.hpp"
#include "base64.hpp"

int main()
{
	std::string public_rsa_key = "MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAkMII8UKPYcguBeyApMD4652TENRYKhFyCL+/UKDuhCWNX8xVeV2pAEniA1QF5xHAv0U8n5vDwIbtjY/Y44PYrRQ2FVjccXBTvkjwOoGaFcM7hCTSm69u8tPi1eL3OGs24FVV7Sf00yGcUNDBX9ng27gF5VGyBXheAT8RozHEIE8ioHKk3z09A5FQlCPUiPqBvX9VI8L6adjQaDvPvDViOMflgl8CFgByy8su8T/RWmnNg+7wIbOwdSR7jgZFhBFIgy+FAoiVGbxtNvLu3HTsIRlGwgDAWdF7DgNL89peIdiYyjKc+PfYiK4FEM6az/jNVMuRNzqWr58Fpu0XHITlSMPQw4OMJIuKjUx1aH9b4e9cekNsI8c0seX9KUY/kjz3ShVJhjXyuW5tvWjTAEbs7kEk/4o7R4BGcprRFlOaJhyKZoQtqxjSfMO+LpSv7pEm/VoC8WkeH4B9E09fEnx47yQj2zHPah7qw/jGFb0+lDZJjNANcuFLCbtGVNMBJWXyh7loxzaTt2nEmzgNXfABUhUA/o/m6BVpa/CsZMrLBwjypn3EDaj6G6wDlY2lTflii33QO+nnHYokJJVVdQQQv0wEtp3vP/uhjCcRS3rd4Ja+PWCWEdme+83By4HaP0n9MBG7Z9BNA+ro/qFAN/69HDT4vD6UdX213z7/bNQWozkCAwEAAQ==";

	std::string plaintext = "super secret message!";
	std::cout << "message: " << plaintext << std::endl << std::endl;;
	std::cout << "message encrypted and encoded into base64:" << std::endl << std::endl;
	std::cout << crypto::rsa_encrypt(plaintext, public_rsa_key);
}