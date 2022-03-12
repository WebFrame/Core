#include <ios>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>

#include <thread>
#include <filesystem>
#include <type_traits>
#include <map>
#include <set>
#include <string>
#include <functional>
#include <utility>
#include <chrono>
#include <regex>

#include <inja/inja.hpp>

#include <webframe/respond_manager.hpp>
#include <webframe/file.hpp>
#include <webframe/host.h>

namespace webframe
{
class webframe
{
private:
	struct cmp
	{
		bool operator()(const auto &a, const auto &b) const { return (&a) < (&b); }
	};
	std::map<
		std::pair<std::vector<std::string>, // var type
				  std::regex>,				// regex
		responser,
		cmp>
		routes;

	static std::pair<std::vector<std::string>, // var type
					 std::regex>			   // regex
	convert_path_to_regex(std::string str, webframe& app)
	{
		static const std::string regexAnyChar = "A-Za-z_%0-9.-";
		std::vector<std::string> v;
		std::string format = "^";
		for (size_t i = 0; i < str.size(); i++)
		{
			if (str[i] == '{')
			{
				std::string var_type;
				for (i++; i < str.size() and str[i] != '}'; i++)
				{
					if (str[i] == ':')
						break;
					var_type += str[i];
				}
				if (var_type == "")
					var_type = "string";
				if (str[i] == '}')
				{
					i -= var_type.size() + 1;
					var_type = "string";
				}
				v.push_back(var_type);
				format += "(";
				std::string curr_val_regex;
				for (i++; i < str.size() and str[i] != '}'; i++)
				{
					if (str[i] == ':')
						break;
					curr_val_regex += str[i];
				}
				if (curr_val_regex == "string" or curr_val_regex == "text")
					curr_val_regex = "[" + regexAnyChar + "]+";
				if (curr_val_regex == "char" or curr_val_regex == "symbol")
					curr_val_regex = "[" + regexAnyChar + "]";
				if (curr_val_regex == "digit")
					curr_val_regex = "[0-9]";
				if (curr_val_regex == "number")
					curr_val_regex = "[1-9][0-9]*";
				if (curr_val_regex == "path")
					curr_val_regex = "[" + regexAnyChar + "\\/]+";
				format += curr_val_regex;
				format += ")";
			}
			else if (str[i] == '/') {
				format += "\\/";
			}
			else
			{
				format += str[i];
			}
		}
		format += "$";
		app.logger << format << '\n';
		return {v, std::regex(format)};
	}

	SynchronizedFile logger;
	SynchronizedFile errors;
	SynchronizedFile performancer;
	std::string template_dir;
	inja::Environment env;

	std::map<std::string, responser> responses;

public:
	webframe()
	{
		performancer = SynchronizedFile(std::clog);
		logger = SynchronizedFile(std::clog);
		errors = SynchronizedFile(std::cout);
		template_dir = ".";

		this->handle("404", [&](std::string path) {
			return "Error 404: " + path + " was not found.";
		})
		.handle("500", [&](std::string reason) {
			return "Error 500: Internal server error: " + reason + ".";
		});
	}

private:
    static constexpr const char* strCodes[] = {"0","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","100","101","102","103","104","105","106","107","108","109","110","111","112","113","114","115","116","117","200","201","202","203","204","205","206","207","208","209","210","211","212","213","214","215","216","217","300","301","302","303","304","305","306","307","308","309","310","311","312","313","314","315","316","317","400","401","402","403","404","405","406","407","408","409","410","411","412","413","414","415","416","417","500","501","502","503","504","505","506","507","508","509","510","511","512","513","514","515","516","517"};
    static constexpr unsigned int codes[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,300,301,302,303,304,305,306,307,308,309,310,311,312,313,314,315,316,317,400,401,402,403,404,405,406,407,408,409,410,411,412,413,414,415,416,417,500,501,502,503,504,505,506,507,508,509,510,511,512,513,514,515,516,517};
public:
    static constexpr int init(unsigned int code = 0) 
    {
        if (code >= sizeof(strCodes)/sizeof(const char*)) return 0;
        if (code == 0)
        {
            mime_types::get_mime_type(".zip");
            string_to_method(method_to_string(method::GET    ));
            string_to_method(method_to_string(method::HEAD   ));
            string_to_method(method_to_string(method::POST   ));
            string_to_method(method_to_string(method::PUT    ));
            string_to_method(method_to_string(method::DDELETE));
            string_to_method(method_to_string(method::CONNECT));
            string_to_method(method_to_string(method::OPTIONS));
            string_to_method(method_to_string(method::TRACE  ));
            string_to_method(method_to_string(method::PATCH  ));
        }
        http_codes::get_reason_by_code(codes[code]); 
        http_codes::get_reason_by_code(strCodes[code]);
        return init(code + 1);
    }

	template <typename F>
	webframe &handle(std::string code, F _res)
	{
		const auto res = wrap(_res);
		responses[code] = responser(res);
		return *this;
	}

	template <typename Ret, typename... Ts>
	webframe &handle(std::string code, std::function<Ret(Ts...)> const &res)
	{
		responses[code] = responser(res);
		return *this;
	}

	auto get_routes() const
	{
		return this->routes;
	}

	webframe &set_performancer(std::ostream &_performancer = std::clog)
	{
		performancer = SynchronizedFile(_performancer);
		return *this;
	}

	webframe &set_logger(std::ostream &_logger = std::clog)
	{
		logger = SynchronizedFile(_logger);
		return *this;
	}

	webframe &set_error_logger(std::ostream &_logger = std::clog)
	{
		errors = SynchronizedFile(_logger);
		return *this;
	}

	webframe &set_static(std::string path, std::string alias)
	{
		std::filesystem::path p = std::filesystem::relative(path);
		this->route(alias + "/{path}", [&path, this](std::string file) {
			std::cout << "Requested file: " << path + "/" + file << std::endl;
			return this->get_file(path + "/" + file);
		});
		return *this;
	}

	webframe &set_templates(std::string path)
	{
		this->template_dir = path;
		return *this;
	}

	response get_file(std::string path)
	{
		std::string ext = std::filesystem::path(path).extension().string();
		const std::string mime = mime_types::get_mime_type(ext.c_str()).data();
		std::ifstream ifs(path, std::ios::in | std::ios::binary);
		if (not ifs.is_open())
		{
			path_vars p;
			p += path_vars::var(path, "string");
			return this->responses.at("404").call("1.1", p);
		}
		else
		{
			std::ostringstream oss;
			oss << ifs.rdbuf();
			std::string content(oss.str());
			std::map<std::string, std::string> m = {
				{"Content-type", mime},
				{"Accept-Ranges", "bytes"},
				{"Cache-Control", "public, max-age=10"},
				{"Connection", "keep-alive"},
				{"Keep-Alive", "timeout=5"}
			};
			return response(status_line("1.1", "200"), m, content);
		}
	}

	response render(std::string path, inja::json params = {})
	{
		path = this->template_dir + "/" + path;
		try
		{
			return response(this->env.render_file(path, params));
		}
		catch (...)
		{ 
			// file not found
			return this->responses.at("404").call("1.1", path_vars() += path_vars::var(path, "string"));
		}
	}

	template <typename Ret, typename... Ts>
	webframe &route(std::string path, std::function<Ret(Ts...)> const &res)
	{
		auto x = convert_path_to_regex(path, *this);
		if (routes.find(x) == routes.end())
			routes[x] = responser(res);
		else // rewriting path
			routes[x] = responser(res);
		return *this;
	}

	template <typename F>
	webframe &route(std::string path, F _res)
	{
		const auto res = wrap(_res);
		auto x = convert_path_to_regex(path, *this);
		if (routes.find(x) == routes.end())
			routes[x] = responser(res);
		else // rewriting path
			routes[x] = responser(res);
		return *this;
	}

	response respond(const std::string &path, const std::string& http = "1.1")
	{
		std::smatch pieces_match;
		for (const auto &s : routes)
		{
			std::regex pieces_regex(s.first.second);
			if (std::regex_match(path, pieces_match, pieces_regex))
			{
				std::ssub_match sub_match = pieces_match[0];
				std::string piece = sub_match.str();
				if (piece == path)
				{
					path_vars params;
					for (size_t i = 1; i < pieces_match.size(); ++i)
					{
						sub_match = pieces_match[i];
						piece = sub_match.str();
						params += path_vars::var(piece, s.first.first[i - 1]);
					}
					return s.second.call(http, params);
				}
			}
		}
		path_vars p;
		p += path_vars::var(path, "string");
		return responses.at("404").call(http, p);
	}

	inline response respond(const request &req, const std::string& http = "1.1")
	{
		return respond(req.uri, http);
	}

	inline response respond(const char *p, const std::string& http = "1.1")
	{
		return respond(std::string(p), http);
	}

private:

	static std::chrono::duration<double, std::milli> timer(const std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::duration<long long int, std::ratio<1, 1000000000> > > start) 
	{
		return std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start);
	}

	int responder(int socket, std::function<void()> callback)
	{
		const std::size_t capacity = 1024; 
		char data[capacity];
		int n = 0;
		request r;
		try
		{
			do
			{
				n = RECV(socket, data, capacity, 0);
				if(n < 0) break;

				auto state = r.loadMore(data, n);

				if (state == LoadingState::LOADED) // Headers are loaded, the buffer is not full and the last char is \0 -> body is filled
					break;
			} while (n > 0);
		}
		catch (std::exception& e)
		{
			this->logger << "Reading Exception: " << e.what() << "\n";
		}
		try
		{
			const auto t1 = std::chrono::high_resolution_clock::now();
			{
				response res;
				try
				{
					res = this->respond(r, r.http);
				}
				catch (std::exception &e)
				{
					throw e;
					//res = this->responses.at("500").call(r.http, path_vars() += {std::string(e.what()), "string"});
				}
				const std::string& response = res.to_string();
				const size_t responseSize = response.size();
				SEND(socket, response.c_str(), responseSize, 0);
				
				this->performancer << r.uri << ": " << timer(t1).count() << "\n";
				CLOSE(socket);
			}
		}
		catch (std::exception& e)
		{
			this->logger << "Responding Exception: " << e.what() << "\n";
		}
		if (callback)
			callback();	
		return 0;
	}

	void listener_host(const char* PORT, const std::function<bool()>& accept_more, const std::function<void()>& callback)
	{
		// Variables for writing a server. 
		/*
		1. Getting the address data structure.
		2. Openning a new socket.
		3. Bind to the socket.
		4. Listen to the socket.
		5. Accept Connection.
		6. Receive Data.
		7. Close Connection.
		*/
		int status;
		struct addrinfo hints, *res;
		int listner;
		
		// Before using hint you have to make sure that the data structure is empty 
		memset(&hints, 0, sizeof hints);
		// Set the attribute for hint
		hints.ai_family = AF_UNSPEC; // We don't care V4 AF_INET or 6 AF_INET6
		hints.ai_socktype = SOCK_STREAM; // TCP Socket SOCK_DGRAM 
		hints.ai_flags = AI_PASSIVE;

		// Fill the res data structure and make sure that the results make sense. 
		status = getaddrinfo(NULL, PORT, &hints, &res);
		this->logger << "status" << status << "\n";
		if (status != 0)
		{
			this->logger << "getaddrinfo error: " << gai_strerror(status) << "\n";

		}

		// Create Socket and check if error occured afterwards
		listner = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		this->logger << "listner" << listner << "\n";
		if (listner < 0)
		{
			fprintf(stderr, "socket error: %s\n", gai_strerror(status));
		}

		// Bind the socket to the address of my local machine and port number 
		status = bind(listner, res->ai_addr, res->ai_addrlen);
		this->logger << "status2" << status << "\n";
		if (status < 0)
		{
			fprintf(stderr, "bind: %s\n", gai_strerror(status));
		}

		status = listen(listner, 1);
		if (status < 0)
		{
			fprintf(stderr, "listen: %s\n", gai_strerror(status));
		}

		// Free the res linked list after we are done with it	
		freeaddrinfo(res);

		// We should wait now for a connection to accept
		int new_conn_fd = -1;
		struct sockaddr_storage client_addr;
		socklen_t addr_size;
		char s[INET6_ADDRSTRLEN]; // an empty string 

		// Calculate the size of the data structure	
		addr_size = sizeof client_addr;

		while (accept_more()) {
			// Accept a new connection and return back the socket desciptor 
			new_conn_fd = ACCEPT(listner, (struct sockaddr *) & client_addr, &addr_size);
			if (new_conn_fd < 0)
			{
				this->logger << "accept/ error: " << gai_strerror(new_conn_fd) << "\n";
				continue;
			}
			 
			if (!accept_more()) break;

			inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *) &client_addr), s, sizeof s);
			this->logger << "I am now connected to " << s << " \n";
			status = responder(new_conn_fd, callback);
			if (status == -1)
			{
				CLOSE(new_conn_fd);
			}
		}
		// Close the socket before we finish
		CLOSE(new_conn_fd);
	}
	
	struct thread
	{
	private:
		std::mutex m;
		void unlock() 
		{
			m.unlock();
		}
		void lock() 
		{
			m.lock();
		}
	public:
		thread() 
		{
			m.unlock();
		}

		void join(std::function<void()> f) 
		{
			this->lock();
			f();
			this->unlock();
		}

		void detach(std::function<void()> f) 
		{
			std::thread([&]() {
				this->lock();
				f();
				this->unlock();
			}).detach();
		}
	};
public:
	bool run(const char* PORT, const unsigned int cores, bool limited = false, unsigned int requests = -1) 
	{
		#ifdef _WIN32
			//----------------------
			// Initialize Winsock.
			WSADATA wsaData;
			this->logger << "Startup called\n";
			int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
			this->logger << "Startup finished " << iResult << "\n";
			if (iResult != NO_ERROR) {
				this->logger << "WSAStartup failed with error: " << iResult << "\n";
				// return;
			}
		#endif

		const unsigned int threads = std::min(cores, requests);
		thread* threads_ptr = new thread [threads];
		try 
		{
			for (unsigned int thread = 0 ; thread < threads ; thread ++) 
			{
				this->logger << "Creating thread " << thread+1 << "\n";
				if(limited)
				{
					this->logger << "Detaching --requests thread " << thread+1 << "\n";
					threads_ptr[thread].join([&]() {
						listener_host(PORT, [&requests](){ return requests != 0; }, [this, &requests]() {
							this->logger << "--requests = " << requests - 1 << "\n";
							requests--;
						});
					});
				}
				else {
					threads_ptr[thread].detach([&, this]() {
						listener_host(PORT, [](){ return true; }, [this, &thread]() {
							this->logger << "Callback from thread " << thread << "\n";
						});
					});
				}
			}
			while(!limited || requests != 0) {}
			#ifdef _WIN32
				WSACleanup();
			#endif
			return true;
		}
		catch (std::exception& e)
		{
			this->logger << "Exception: " << e.what() << "\n";
			return false;
		}
	}
};
} // namespace webframe
