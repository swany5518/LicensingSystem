#include <string>
#include <Windows.h>

namespace util
{
	inline std::string get_current_path()
	{
		char result[MAX_PATH];
		auto filepath = std::string(result, GetModuleFileNameA(NULL, result, MAX_PATH));
		return filepath.substr(0, filepath.length() - std::distance(filepath.rbegin(), std::find(filepath.rbegin(), filepath.rend(), '\\')));
	}
}