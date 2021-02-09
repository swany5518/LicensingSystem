#include <thread>

#include "crypto.hpp"
#include "base64.hpp"
#include "network.hpp"
#include "sha256.hpp"
#include "client_gui.hpp"

int main()
{
	std::thread network{ network::api::network_api_thread };

	client_gui::start();

	network::api::network_thread_should_run = false;
	network.join();
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	main();
}