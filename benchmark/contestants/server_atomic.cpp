#include <webframe/webframe.hpp>
#include <iostream>
#include <sstream>
#include <stdlib.h>

int main(int argc, char** argv)
{
	constexpr long long fasten = webframe::webframe::init();
	std::atomic<long long> pass{fasten};
	webframe::webframe app;
	app.route("/{number}/2", [&](long long steps) {
			for (long long i = 0; i < (1 << steps); i++)
			{
				pass.fetch_add(rand(), std::memory_order_relaxed);
			}
			return "Hello World!";
		})
		.route("/{number}/1", [&](long long steps) {
			for (long long i = 0; i < (1 << steps); i++)
			{
				pass.fetch_add(1, std::memory_order_relaxed);
			}
			return "Hello World!";
		});
	const char* port = argv[1];
	app.run(port, 1).wait_end(port);
}
