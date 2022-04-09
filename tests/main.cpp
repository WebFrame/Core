#include <moka/moka.h>
#include <webframe/webframe.hpp>

#include <stdio.h>
std::ostream* nil;
const unsigned char cores = std::thread::hardware_concurrency();

Moka::Context all ("Web++ framework - testing", [](Moka::Context& it) {
	it.should("response with 200 and the testing string", []() {
		const std::string text = "sample";
	
		webframe::webframe app;
		app
		.set_logger(*nil)
		.set_error_logger(*nil)
		.route ("/", [text]() { // static setup
				return webframe::response (webframe::status_line ("200"), {{"Content-Type", "text/html; charset=utf-8"}}, text);
		});
		
		auto r = (*app.get_routes().begin()).second.call("1.1", "", webframe::path_vars());
		must_equal("HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\n\n" + text, r.to_string());
	});
	it.should("response with 1.1/201 and the username", []() {
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
		auto r = (*app.get_routes().begin()).second.call("1.1", "", params);
		must_equal("HTTP/1.1 201 Created\nContent-Type: text/html; charset=utf-8\n\n" + username, r.to_string());
	});
	it.should("response with 1.1/201, the username and a custom header", []() {
		const std::string username = "sample username", testing_header="testing header";
	
		webframe::webframe app;
		app
		.set_logger(*nil)
		.set_error_logger(*nil)
		.route ("/{.*}", [testing_header](std::string username) { // static setup
				return webframe::response (webframe::status_line ("1.1", "201"), {{"Custom-header", testing_header}, {"Content-Type", "text/html; charset=utf-8"}}, username);
		});
		try {
			auto params = webframe::path_vars();
			params += {username, "string"};
			auto r = (*app.get_routes().begin()).second.call("1.1", "", params);
			must_equal("HTTP/1.1 201 Created\nContent-Type: text/html; charset=utf-8\nCustom-header: " + testing_header + "\n\n" + username, r.to_string());
		} catch(std::exception& e) {
			must_equal(1, 0);
			std::cout << e.what() << std::endl;
		}
	});
	it.should("test whole functionality", [](){
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
		.run("8887", cores, 1, 1)
		.wait_start();

		system("curl http://localhost:8887/ > ./bin/log/curl.txt 2>> ./bin/log/log.txt &");
		
		app.wait_down();
		
		std::ifstream fin ("./bin/log/curl.txt");
		std::string response; 
		std::getline(fin, response);
		must_equal(response, "<h1>Hello, World!</h1>");
	});
	
	it.should("get performance data", [](){
		volatile int count = 0;
		std::filebuf performance;
		performance.open ("./bin/log/performance.txt",std::ios::out);
		std::ostream performancer (&performance);
		webframe::webframe app;
		app
		.set_logger(*nil)
		.set_error_logger(*nil)
		.set_performancer(performancer)
		.route ("/{number}", [&count](int steps) {	
			for (volatile int i = 0; i < (1 << steps); i++)
			{
				count++;
			}
			return "Hello, World!";
		})
		.run("8889", cores, 1, 31)
		.wait_start();

		char buffer [3];
		std::string command;
		for (int i = 0 ; i <= 30 ; i ++)
		{
			command = std::string("curl http://localhost:8889/") + std::string(webframe::itoa(i, buffer, 10)) + " > ./bin/log/curl.txt 2>> ./bin/log/log.txt &";
			system(command.c_str());
		}
		
		app.wait_down();

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
		must_be_less (sum, 1);
	});
});

int main ()
{
	constexpr int _ = webframe::webframe::init();
	
	std::filebuf fb;
	fb.open ("./bin/log/buffer.txt", std::ios::out);
	std::ostream nill (&fb);

	nil = &nill;

	std::cout << _ << "===============================  Testing  ===============================" << _ << "\n";

	Moka::Report report;
	all.test(report);
	report.print();

	return 0;
}