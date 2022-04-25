#pragma once

#include <moka/moka.h>
#include <webframe/webframe.hpp>

class response_of {
    std::string response;
    double time;
public:
    response_of(const std::string& url) {
        const std::string command = "curl  -w \"%{time_total}\" " + url + " -o ./bin/log/curl.txt > ./bin/log/time.txt 2> ./bin/log/curl_logs.txt";
      
        int code __attribute__((unused)) = system(command.c_str());
       
        std::ifstream fin;
        fin.open("./bin/log/curl.txt");
        getline(fin, response);
        fin.close();

        fin.open("./bin/log/time.txt");
        fin >> time;
        fin.close();
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

void testCase_IntegrationTests (Moka::Report& report) {
    const unsigned int requests = 31;
	webframe::webframe app;

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
				volatile int count = 0;
				app
				.set_logger(nil)
				.set_error_logger(nil)
				.set_performancer(performancer)
				.route ("/", []() { // static setup
					return webframe::response (webframe::status_line ("1.1", "200"), {{"Content-Type", "text/html; charset=utf-8"}}, "<h1>Hello, World!</h1>");
				})
				.route ("/{number}", [&count](int steps) {	
					for (int i = 0; i < (1 << steps); )
					{
						count = count + 1; 
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
		});
	});
	integration_tests.test(report);
}