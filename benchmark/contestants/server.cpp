#include <webframe/webframe.hpp>
#include <iostream>
#include <sstream>
#include <stdlib.h>

int main(int argc, char** argv)
{
	constexpr int fasten = webframe::webframe::init();
	int pass = fasten;
	webframe::webframe app;
	app.route("/{number}/2", [&](int steps) {
			for (int i = 0; i < (1 << steps); i++)
			{
				pass = pass + rand();
			}
			return "Hello World!";
		})
		.route("/{number}/1", [&](int steps) {
			for (int i = 0; i < (1 << steps); i++)
			{
				__asm__("");
			}
			return "Hello World!";
		});
	const char* port = argv[1];
	std::cout << port << std::endl;
	app.run(port, 1).wait_end(port);
}
