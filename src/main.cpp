
#include <signal.h>

#include <ossia/network/local/local.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>

#include "c24_protocol.hpp"


static bool running = true;

static void SIGINT_handler(int dummy)
{
	std::cout << std::endl << "Quiting..." << std::endl;
	running = false;	
}

static void usage(const char *argv0)
{
	std::cout << "usage : " << argv0 << " <osc_port> <ws_port>" << std::endl;
}

int main(int argc, char **argv)
{
	if (argc != 3) {
		usage(argv[0]);
		return 0;
	}

	signal(SIGINT, SIGINT_handler);	//	ensure that everything will be closed/freed properly

	auto protocol = std::make_unique<ossia::net::multiplex_protocol>();
	auto& multiplex = *protocol;

	ossia::net::generic_device c24{std::move(protocol), "digidesign_c24"};

	multiplex.expose_to(std::make_unique<ossia::oscquery::oscquery_server_protocol>(atoi(argv[1]), atoi(argv[2])));
	multiplex.expose_to(std::make_unique<c24_device::digidesign_c24_protocol>());

	while(running)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return 0;
}
