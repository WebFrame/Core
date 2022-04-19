#include <moka/moka.h>
#include <webframe/webframe.hpp>

#include <stdio.h>
std::ostream* nil;
const unsigned char cores = std::thread::hardware_concurrency();

inline std::string get_response(const std::string& url) {
	const std::string command = "python ./tests/get.py " + url + " > ./bin/log/curl.txt";
	int code __attribute__((unused)) = system(command.c_str());
	std::ifstream fin("./bin/log/curl.txt");
	std::string output;
	getline(fin, output);
	fin.close();
	return output;
} 

Moka::Context all ("WebFrame - testing", [](Moka::Context& it) {
	it.describe("responser", [](Moka::Context& it) {
		it.should("respond with status code 200 by default", []() {
			webframe::webframe app;
			app
			.set_logger(*nil)
			.set_error_logger(*nil)
			.route ("/", []() { // static setup
				return "";
			});
			
			auto r = (*app.get_routes().begin()).second.call("1.1", "", webframe::path_vars()).to_string();
			must_contain("HTTP/1.1 200 OK", r, "The server did not respond with 200 OK by default.");
		});
		it.should("respond with status code 500 when set to", []() {
			webframe::webframe app;
			app
			.set_logger(*nil)
			.set_error_logger(*nil)
			.route ("/", []() { // static setup
				return webframe::response (webframe::status_line ("500"), {{"Content-Type", "text/html; charset=utf-8"}}, "");
			});
			
			auto r = (*app.get_routes().begin()).second.call("1.1", "", webframe::path_vars()).to_string();
			
			must_contain("HTTP/1.1 500 Internal Server Error", r, "The server did not respond with the status code.");
		});
		
		it.should("respond with the correct html", []() {
			const std::string text = "sample";
			webframe::webframe app;
			app
			.set_logger(*nil)
			.set_error_logger(*nil)
			.route ("/", [text]() { // static setup
				return text;
			});
			
			auto r = (*app.get_routes().begin()).second.call("1.1", "", webframe::path_vars()).to_string();
			
			must_contain(text, r, "The server did not respond with the correct body.");
		});

		it.should("respond with the given path variable value", []() {
			const std::string username = "sample username";
			webframe::webframe app;
			app
			.set_logger(*nil)
			.set_error_logger(*nil)
			.route ("/{.*}", [](std::string username) { // static setup
				return webframe::response (webframe::status_line ("1.1", "201"), {{"Content-Type", "text/html; charset=utf-8"}}, username);
			});
			auto params = webframe::path_vars();
			params += {username, "string"};

			auto r = (*app.get_routes().begin()).second.call("1.1", "", params).to_string();
			
			must_contain(username, r, "The server did not respond with the correct body when path vavriable is passed.");
		});
		it.should("respond with a custom header", []() {
			const std::string username = "sample username", testing_header="testing header";
		
			webframe::webframe app;
			app
			.set_logger(*nil)
			.set_error_logger(*nil)
			.route ("/{.*}", [testing_header](std::string username) { // static setup
				return webframe::response (webframe::status_line ("1.1", "201"), {{"Custom-header", testing_header}, {"Content-Type", "text/html; charset=utf-8"}}, username);
			});
			auto params = webframe::path_vars();
			params += {username, "string"};
			
			auto r = (*app.get_routes().begin()).second.call("1.1", "", params).to_string();
			
			must_contain("Custom-header: " + testing_header, r, "The server did not include the custom header to the response.");
		});
	});
	
	it.describe("The web server", [](Moka::Context& it) {
		it.should("respond to web requests", [](){
			std::filebuf performance;
			performance.open ("./bin/log/performance.txt",std::ios::out);
			std::ostream performancer (&performance);
			webframe::webframe app;
			app
			.set_logger(*nil)
			.set_error_logger(*nil)
			.set_performancer(performancer)
			.route ("/", []() { // static setup
				return webframe::response (webframe::status_line ("1.1", "200"), {{"Content-Type", "text/html; charset=utf-8"}}, "<h1>Hello, World!</h1>");
			})
			.run("8887", 1, true, 1)
			.wait_start("8887");

			const std::string response = get_response("http://localhost:8887/");
			app.wait_end("8887");

			must_be_equal(response, "<h1>Hello, World!</h1>", "Incorect response.");
		});
		
		it.should("respond to small and big requests for less than 1 nonosecond per action", [](){
			volatile int count = 0;
			volatile int i;
			std::filebuf performance;
			performance.open ("./bin/log/performance.txt",std::ios::out);
			std::ostream performancer (&performance);
			webframe::webframe app;
			app
			.set_logger(*nil)
			.set_error_logger(*nil)
			.set_performancer(performancer)
			.route ("/{number}", [&count, &i](int steps) {	
				for (i = 0; i < (1 << steps); )
				{
					count = count + 1; i = i + 1;
				}
				return "Hello, World!";
			})
			.run("8889", cores, true, 31)
			.wait_start("8889");

			char buffer [3];
			for (int i = 0 ; i <= 30 ; i ++)
			{
				const std::string response = get_response("http://localhost:8889/" + std::string(webframe::itoa(i, buffer, 10)));

				must_be_equal(response, "Hello, World!", "Incorect response for http://localhost:8889/" + std::string(buffer) + ".");
			}
			
			app.wait_end("8889");

			std::ifstream fin ("./bin/log/performance.txt");
			double sum = 0;
			int n;
			for (n = 0 ; !fin.eof() ; n ++)
			{
				double a;
				std::string str;
				fin >> str >> a;
				sum += a * 1000000;
			}
			sum /= ((1ll<<31) - 1ll);
			std::ofstream fout;
			fout.open("./bin/log/performance_summary.txt", std::ios::trunc);
			fout << sum << " nanoseconds avg. per operation in the resolver\n";
			must_be_less (sum, 1, "The time per operation is too high.");
		});
	});
});

int main ()
{
	static_assert(webframe::webframe::init(), "constexpr initiation failed");
	
	std::filebuf fb;
	fb.open ("./bin/log/buffer.txt", std::ios::out);
	std::ostream nill (&fb);

	nil = &nill;

	std::cout << "===============================  Testing  ===============================" << "\n";

	Moka::Report report;
	all.test(report);
	report.print();

	return 0;
}