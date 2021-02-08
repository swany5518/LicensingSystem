#include <thread>

#include "crypto.hpp"
#include "base64.hpp"
#include "network.hpp"
#include "sha256.hpp"
#include "client_gui.hpp"

int main()
{
	//std::thread network{ network::api::network_api_thread };

	network::api::product_list.push_back(products::product{ "id1", "test 1", "up", 10000, false });
	network::api::product_list.push_back(products::product{ "id2", "test 2", "up", 9999, false });
	network::api::product_list.push_back(products::product{ "id3", "test 3", "up", 13456, false });
	network::api::product_list.push_back(products::product{ "id4", "test 4", "up", 13456, false });
	network::api::product_list.push_back(products::product{ "id5", "test 5", "up", 13456, false });
	network::api::product_list.push_back(products::product{ "id6", "test 6", "up", 13456, false });

	client_gui::start();

	//network::api::network_thread_should_run = false;
	//network.join();
}