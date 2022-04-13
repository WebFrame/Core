#include <webframe/webframe.hpp>
#include <iostream>
#include <sstream>
#include <stdlib.h>

int main()
{
	constexpr int fasten = webframe::webframe::init();
	volatile int pass = fasten;
	webframe::webframe app;
	app.route("/{number}/2", [&](int steps) {
			for (int i = 0; i < (1 << steps); i++)
			{
				pass = pass + rand();
			}
			return "Hello World!";
		})
		.route("/{number}", [&](int steps) {
			for (int i = 0; i < (1 << steps); i++)
			{
				pass = pass + 1;
			}
			return "Hello World!";
		});
	const char* port = "8888";
	app.run(port, 1).wait_end(port);
}
