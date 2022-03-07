#include <webframe/webframe.hpp>
#include <iostream>
#include <sstream>
#include <stdlib.h>

int main()
{
	constexpr int fasten = webframe::webframe::init();
	int pass = fasten;
	webframe::webframe app;
	app.set_static("./src/static", "/static")
		.set_templates("./src/static/templates")
		.handle("404", [&](std::string path) {
			return "Error 404: " + path + " was not found.";
		})
		.handle("500", [&](std::string reason) {
			return "Error 500: Internal server error: " + reason + ".";
		})
		.route ("/", []() { // static setup
				return webframe::response (webframe::status_line ("1.1", "200"), {{"Content-Type", "text/html; charset=utf-8"}}, "<h1>Hello, World!</h1>");
		})
		.route("/{text}", [&](std::string user) {
			return app.render("template.html", {{"username", user}});
		})
		.route("/favicon.ico", [&]() {
			return "";
		})
		.route("/{number}", [&](int steps) {
			for (int i = 0; i < (1 << steps); i++)
			{
				pass += rand();
			}
			return "Hello World!";
		})
		.route("/{number}/2", [&](int steps) {
			for (int i = 0; i < (1 << steps); i++)
			{
				pass++;
			}
			return "Hello World!";
		})
		.route("/{number}/3", [&](int steps) {
			throw std::logic_error("asdf");
			return "asdf";
		});
	const unsigned short port = 8888;
	const unsigned char cores = ((std::thread::hardware_concurrency() - 1 > 0) ? (std::thread::hardware_concurrency() - 1) : 1);
	app.run(port, cores);
}
