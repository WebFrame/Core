#include <iostream>
#include <webframe/webframe.hpp>
#include "./routes/home.hpp"

int main(int args, char** argv) {
	webframe::webframe app;

    // setup
    app.extend_with(home);
    
    // start
	const char* port = "8889";
	const unsigned char cores = ((std::thread::hardware_concurrency() - 1 > 0) ? (std::thread::hardware_concurrency() - 1) : 1);
	app.run(port, cores).wait_end(port);
    return 0;
}