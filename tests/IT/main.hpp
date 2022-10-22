#pragma once

#include <moka/moka.h>
#include <core/core.hpp>

#include <random>
#include <string>
	
class response_of {
	static int custom;
	static constexpr auto chars =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

    std::string response;
    double time;
	std::string current;
public:
    response_of(const std::string& url) {
		custom = custom + 1;
		current = chars [custom];
        const std::string command = "curl -w \"%{time_total}\" " + url + " -o ./bin/log/curl" + current + ".txt > ./bin/log/time" + current + ".txt 2> ./bin/log/curl_logs" + current + ".txt";
      
        const int code [[maybe_unused]] = system(command.c_str());
        {
			std::ifstream fin("./bin/log/curl" + current + ".txt");
			std::stringstream buffer;
			buffer << fin.rdbuf();
			response = buffer.str();
		}
		{
			std::ifstream fin("./bin/log/time" + current + ".txt");
			fin >> time;
		}
    }

    response_of& must_be (const std::string& text, const std::string& message) {
        must_be_equal(response, text, message);
        return *this;
    }

    response_of& might_take_less (double max_time, const std::string& message) {
        would_be_nice_to_be_less(time, max_time, message);
        return *this;
    }
};

int response_of::custom = 0;

void testCase_IntegrationTests (Moka::Report& report) {
    const unsigned int requests = 31;
	webframe::core::application app;

	std::filebuf fileBuffer;
	fileBuffer.open ("./bin/log/buffer-IT.txt", std::ios::out);
	std::ostream nil (&fileBuffer);
	
	std::filebuf performance;
	performance.open("./bin/log/performance.txt", std::ios::out | std::ios::trunc);
	std::ostream performancer (&performance);

    const std::string pathParams[] = {"0","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19","20","21","22","23","24","25","26","27","28","29","30","31"};

	Moka::Context integration_tests ("WebFrame - integration testing", [&requests, &app, &nil, &performancer, &pathParams](Moka::Context& it) {
		it.describe("The web server", [&requests, &app, &nil, &performancer, &pathParams](Moka::Context& it) {
			it.setup([&app, &nil, &performancer]() {
				app
				.set_logger(nil)
				.set_warner(nil)
				.set_error_logger(nil)
				.set_performancer(performancer)
				.route ("/", []() {
					return webframe::core::response (webframe::core::status_line ("1.1", "200"), {{"Content-Type", "text/html; charset=utf-8"}}, "<h1>Hello, World!</h1>");
				})
				.route ("/{number}", [](int steps) {	
					for (int i = 0; i < (1 << steps); )
					{
						__asm__("");
						i = i + 1;
					}
					return "Hello, World!";
				})
				.run("8889", 1)
				.wait_start("8889");
			});

			it.should("respond to web requests", [](){
                response_of("http://localhost:8889/").must_be("<h1>Hello, World!</h1>", "Incorect response for http://localhost:8889/.");
            });
			
			for (unsigned int number = 0 ; number < requests ; number ++) {
				it.should("respond to request with 2^" + pathParams[number] + " operations for less than 1 nanosecond per action", [&requests, number, &pathParams]() {
					response_of("http://localhost:8889/" + pathParams[number])
                        .must_be("Hello, World!", "Incorect response for http://localhost:8889/" + pathParams[number] + ".")
                        .might_take_less(15 * 1e-6 * (1ll << number), "The time per operation is too much."); // 15 nanosec per operation
				});
			}

			it.teardown([&app]() {
				app.request_stop("8889");
			});
		});
	});
	integration_tests.test(report);
}