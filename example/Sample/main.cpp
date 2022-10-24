#include <core/core.hpp>
#include <iostream>
#include <sstream>
#include <atomic>
#include <stdlib.h>

int main()
{
	static_assert(webframe::core::application::init(), "constexpr initiation failed");
	std::atomic<int> pass {0};
	webframe::core::application app;
	app.set_static("./example/Sample/static", "/static")
#ifdef USE_INJA
		.set_templates("./example/Sample/static/templates")
#endif
		.handle("404", [&](const std::string& path) {
			return "Error 404: " + path + " was not found.";
		})
		.handle("500", [&](const std::string& reason) {
			return "Error 500: Internal server error: " + reason + ".";
		})
		.route ("/", [&]() { // static setup
			return webframe::core::response (webframe::core::status_line ("1.1", "200"), {{"Content-Type", "text/html; charset=utf-8"}}, "<h1>Hello, World!</h1>");
		})
		.route("/{text}", [&](const std::string& user) {
			return app.render("template.html", {{"username", user}});
		})
		.route("/favicon.ico", [&]() {
			return "";
		})
		.route("/{number}", [&](int steps) {
			for (int i = 0; i < (1 << steps); i++)
			{
				pass.fetch_add(rand(), std::memory_order_relaxed);
			}
			return "Hello World!";
		})
		.route("/{number}/2", [&](int steps) {
			for (int i = 0; i < (1 << steps); i++)
			{
				pass.fetch_add(1, std::memory_order_relaxed);
			}
			return "Hello World!";
		})
		.route("/{number}/3", [&]([[maybe_unused]] int steps) {
			throw std::logic_error("asdf");
			return "asdf";
		})
		.route("/{number}/4", [&]([[maybe_unused]] const webframe::core::body_t& request_body, [[maybe_unused]] int steps) {
			return "asdf";
		})
		.route("/6", [&]([[maybe_unused]] const webframe::core::body_t& request_body) {
			return "asdf";
		})
		.route("/{text}/7", [&]([[maybe_unused]] const std::string& not_request_body) {
			return "asdf";
		});
	const char* port = "8889";
	const unsigned char cores = ((std::thread::hardware_concurrency() - 1 > 0) ? (std::thread::hardware_concurrency() - 1) : 1);
	app.run(port, cores).wait_end(port);
}
