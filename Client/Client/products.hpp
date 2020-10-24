#include <string>
#include <cstdint>

namespace products
{
	
	struct product
	{
		std::string id;
		std::string name;
		uint64_t seconds_left;
	};
}