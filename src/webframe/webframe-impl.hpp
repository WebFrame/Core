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
#include <future>
#include <optional>

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
		template<typename T>
		bool operator()(const T &a, const T &b) const { return (&a) < (&b); }
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

	std::string template_dir;
	inja::Environment env;

	std::map<std::string, responser> responses;

public:
	SynchronizedFile logger;
	SynchronizedFile errors;
	SynchronizedFile performancer;

	webframe()
	{
		performancer = SynchronizedFile(std::clog);
		logger = SynchronizedFile(std::clog);
		errors = SynchronizedFile(std::cout);
		template_dir = ".";

		this->handle("404", [&](const std::string& path) {
			return "Error 404: " + path + " was not found.";
		})
		.handle("500", [&](const std::string& reason) {
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

	webframe &set_static(const std::string& path, const std::string& alias)
	{
		std::filesystem::path p = std::filesystem::relative(path);
		this->route(alias + "/{path}", [&path, this](const std::string& file) {
			return this->get_file(path + "/" + file);
		});
		return *this;
	}

	webframe &set_templates(const std::string& path)
	{
		this->template_dir = path;
		return *this;
	}

	response get_file(const std::string& path)
	{
		std::string ext = std::filesystem::path(path).extension().string();
		const std::string mime = mime_types::get_mime_type(ext.c_str()).data();
		std::ifstream ifs(path, std::ios::in | std::ios::binary);
		if (not ifs.is_open())
		{
			path_vars p;
			p += path_vars::var(path, "string");
			return this->responses.at("404").call("1.1", "", p);
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

	response render(const std::string& path, inja::json params = {})
	{
		path = this->template_dir + "/" + path;
		try
		{
			return response(this->env.render_file(path, params));
		}
		catch (...)
		{ 
			// file not found
			return this->responses.at("404").call("1.1", "", path_vars() += path_vars::var(path, "string"));
		}
	}

	template <typename Ret, typename... Ts>
	webframe &route(const std::string& path, std::function<Ret(Ts...)> const &res)
	{
		auto x = convert_path_to_regex(path, *this);
		if (routes.find(x) == routes.end())
			routes[x] = responser(res);
		else // rewriting path
			routes[x] = responser(res);
		return *this;
	}

	template <typename F>
	webframe &route(const std::string& path, F _res)
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
					return s.second.call(http, "", params);
				}
			}
		}
		path_vars p;
		p += path_vars::var(path, "string");
		return responses.at("404").call(http, "", p);
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

	int responder(int socket)
	{
		request r;
		try
		{
			const std::size_t capacity = 1024; 
			char data[capacity];
			int n = 0;
			do
			{
				n = RECV(socket, data, capacity, 0);
				if (n < 0)
				{
					break;
				}

				auto state = r.loadMore(data, n);

				if (state == LoadingState::LOADED) // Headers are loaded, the buffer is not full and the last char is \0 -> body is filled
					break;
			} while (n > 0);

			const auto t1 = std::chrono::high_resolution_clock::now();
			response res;
			res = this->respond(r, r.http);
			const std::string& response = res.to_string();
			const size_t responseSize = response.size();
			int status;
			status = SEND(socket, response.c_str(), responseSize, 0);
			if(status == SOCKET_ERROR) {
				return -1;
			}
			this->performancer << r.uri << ": " << timer(t1).count() << "\n";
		}
		catch (std::exception const& e)
		{
			this->logger << "Responding Exception: " << e.what() << "\n";
			response res = this->responses.at("500").call(r.http, "", path_vars() += {std::string(e.what()), "string"});
			const std::string& response = res.to_string();
			const size_t responseSize = response.size();
			SEND(socket, response.c_str(), responseSize, 0);
			return -1;
		}
		return 0;
	}

	void handler(int client, const std::function<void()>& callback)
	{
		int status = this->responder(client);
		callback();
		if (status == -1) 
		{
			CLOSE(client);
		} 
		else 
		{
			CLOSE(client);
		}
	}
	
	struct thread_pool;
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
		bool try_lock() 
		{
			return m.try_lock();
		}
	public:
		std::shared_ptr<int> requestor;

		thread() 
		{
			m.unlock();
		}

		void join(std::shared_ptr<std::function<void(int)>> f, int socket) 
		{
			this->lock();
			std::thread([this, f](int socket) {
				f->operator()(socket);
				this->unlock();
			}, socket).join();
		}

		void detach(std::shared_ptr<std::function<void(int)>> f, int socket) 
		{
			this->lock();
			std::thread([this, f](int socket) {
				f->operator()(socket);
				this->unlock();
			}, socket).detach();
		}

		friend struct thread_pool;
	};

	struct thread_pool 
	{
	private:
		std::shared_ptr<std::vector<std::shared_ptr<thread>>> pool;
		size_t size;
		std::mutex extract;
	public:
		thread_pool(size_t _size) 
		{
			size = _size;
			pool = std::make_shared<std::vector<std::shared_ptr<thread>>>(_size);
			for (size_t i = 0 ; i < _size ; i ++)
			{	
				pool->at(i) = std::make_shared<thread>();
			}
		}

		std::shared_ptr<thread>& get(const size_t index) const 
		{
			return pool->at(index);
		}

		std::shared_ptr<thread>& operator[] (const size_t index) const
		{
			return this->get(index);
		}
		
		std::optional<size_t> get_free_thread() 
		{
			std::lock_guard locker (this->extract);
			for (size_t index = 0 ; index < this->size ; index ++)
			{
				if (pool->at(index)->try_lock())
				{
					pool->at(index)->unlock();
					return index;
				}
			}
			return {};
		}
	};
 
public:
	std::shared_future<void> run(const char* PORT, const unsigned int cores, std::promise<void>* started = nullptr, bool limited = false, unsigned int requests = -1) 
	{
		std::shared_ptr<std::promise<void>> running = std::make_shared<std::promise<void>>();
		
		std::thread([&, this](std::shared_ptr<std::promise<void>> running) {
			#ifdef _WIN32
				//----------------------
				// Initialize Winsock.
				WSADATA wsaData;
				this->logger << "Startup called\n";
				int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
				this->logger << "Startup finished " << iResult << "\n";
				if (iResult != NO_ERROR) {
					this->logger << "WSAStartup failed with error: " << iResult << "\n";
					running->set_value();
					return;
				}
			#endif
			this->logger << "Startup called\n";
	
			const unsigned int threads = std::min(cores, limited ? requests : cores);
			std::shared_ptr<thread_pool> threads_ptr = std::make_shared<thread_pool>(threads);
			
			this->logger << "Thread pool generated\n";	

			int status;
			struct addrinfo hints, *res;
			int listener;
			
			// Before using hint you have to make sure that the data structure is empty 
			memset(&hints, 0, sizeof hints);
			// Set the attribute for hint
			hints.ai_family = AF_INET; // We don't care V4 AF_INET or 6 AF_INET6
			hints.ai_socktype = SOCK_STREAM; // TCP Socket SOCK_DGRAM 
			hints.ai_flags = AI_PASSIVE;
			hints.ai_protocol = 0;          /* Any protocol */
			hints.ai_canonname = NULL;
			hints.ai_addr = NULL;
			hints.ai_next = NULL;

			// Fill the res data structure and make sure that the results make sense. 
			status = getaddrinfo(NULL, PORT, &hints, &res);
			if (status != 0)
			{
				this->logger << "getaddrinfo error: " << gai_strerror(status) << "\n";
				running->set_value();
				return;
			}

			// Create Socket and check if error occured afterwards
			listener = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
			if (listener == -1)
			{
				this->logger << "socket error: " << gai_strerror(status) << "\n";
				running->set_value();
				return;
			}

			// Bind the socket to the address of my local machine and port number 
			status = bind(listener, res->ai_addr, res->ai_addrlen);
			if (status < 0)
			{
				this->logger << "bind error: " << gai_strerror(status) << "\n";
				running->set_value();
				return;
			}

			status = listen(listener, 10);
			if (status < 0)
			{
				this->logger << "listen error: " << gai_strerror(status) << "\n";
				running->set_value();
				return;
			}

			status = nonblock_config(listener);
			if (status < 0)
			{
				this->logger << "nonblocking config error: " << gai_strerror(status) << "\n";
				running->set_value();
				return;
			}

			// Free the res linked list after we are done with it	
			freeaddrinfo(res);

			this->logger << "Listener setup " << listener << "\n";

			started->set_value();

			while (!limited || requests != 0) {
				const std::optional<size_t> thread = threads_ptr->get_free_thread();
				if(!thread)
					continue;

				// Accept a new connection and return back the socket desciptor 
				threads_ptr->get(thread.value())->requestor = std::shared_ptr<int>(new int(ACCEPT(listener, NULL, NULL)));
				if (*threads_ptr->get(thread.value())->requestor == -1)
				{
					continue;
				}

				this->logger << "Requestor " << *threads_ptr->get(thread.value())->requestor << " is getting handled\n";

				struct timeval selTimeout;
				selTimeout.tv_sec = 2;
				selTimeout.tv_usec = 0;
				fd_set readSet;
				FD_ZERO(&readSet);
				FD_SET(*threads_ptr->get(thread.value())->requestor + 1, &readSet);
				FD_SET(*threads_ptr->get(thread.value())->requestor, &readSet);

				this->logger << "Requestor " << *threads_ptr->get(thread.value())->requestor << " is checked if valid\n";
				
				status = SELECT(*threads_ptr->get(thread.value())->requestor + 1, &readSet, nullptr, nullptr, &selTimeout);
				this->logger << "SELECT status is " << status << "\n";
				if (status <= 0)
					continue;

				this->logger << "Requestor " << *threads_ptr->get(thread.value())->requestor << " is still valid\n";

				if (getsockname(*threads_ptr->get(thread.value())->requestor, nullptr, nullptr) < 0 && errno == ENOTSOCK) continue;
				
				this->logger << "Requestor " << *threads_ptr->get(thread.value())->requestor << " is still valid\n";

				this->logger << thread.value() << " thread will handle client " << *threads_ptr->get(thread.value())->requestor << "\n";
				
				threads_ptr->get(thread.value())->detach(std::make_shared<std::function<void(int)>>([this, &limited, &running, &requests](int socket) -> void {
					this->handler (socket, [&limited, &running, &requests]() {
						if (!limited) return;
						requests--;
						if (requests == 0) {
							running->set_value();
						}
					});
				}), *threads_ptr->get(thread.value())->requestor);
			}
			#ifdef _WIN32
				WSACleanup();
			#endif
		}, running).detach();
		return running->get_future().share();
	}
};
} // namespace webframe
