#include <string>
#include <cstdint>
#include <unordered_map>

namespace products
{
	struct product
	{
		std::string id;
		std::string name;
		uint64_t seconds_left;
		bool selected;

		product(std::string id_, std::string name_, uint64_t seconds_left_, bool selected_)
		{
			id = id_;
			name = name_;
			seconds_left = seconds_left_;
			selected = selected_;
		}

		std::string get_file_name()
		{
			return id.substr(0, 8);
		}
	};
}