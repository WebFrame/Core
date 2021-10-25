#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <winsock.h>
#include <windows.h>
#define SO_REUSEPORT SO_BROADCAST
using socklen_t = int;
#include <stopwatch.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include <thread>
#define mingw_stdthread std

/*
#if defined(__cplusplus) && __cplusplus >= 201703L && defined(__has_include)
	#if __has_include(<filesystem>)
		#define GHC_USE_STD_FS
		#include <filesystem>
		namespace fs = std::filesystem;
	#endif
#endif
*/
#ifndef GHC_USE_STD_FS
#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;
#endif

#include <type_traits>
#include <iomanip>
#include <webnetpp/base.hpp>
#include <map>
#include <set>
#include <string>
#include <functional>

#include <utility>
#include <chrono>

#include <ios>
#include <iostream>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <webnetpp/file.hpp>
#include <webnetpp/mime.hpp>
#include <webnetpp/lambda2function.hpp>

#include <jinja2cpp/template.h>

namespace webnetpp
{
class webnetpp;

#include <webnetpp/respond_manager.hpp>

class webnetpp
{
private:
	struct cmp
	{
		bool operator()(const auto &a, const auto &b) const { return (&a) < (&b); }
	};
	std::map<
		std::pair<std::vector<std::string>, // var type
				  std::regex>,				// regex
		responcer,
		cmp>
		routes;

	static std::pair<std::vector<std::string>, // var type
					 std::regex>			   // regex
	convert_path_to_regex(std::string str)
	{
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
					curr_val_regex = "[A-Za-z_%0-9.]+";
				if (curr_val_regex == "char" or curr_val_regex == "symbol")
					curr_val_regex = "[A-Za-z_%0-9.]";
				if (curr_val_regex == "digit")
					curr_val_regex = "[0-9]";
				if (curr_val_regex == "number")
					curr_val_regex = "[1-9][0-9]*";
				if (curr_val_regex == "path")
					curr_val_regex = "[A-Za-z_/%0-9.]+";
				std::cout << curr_val_regex << std::endl;
				format += curr_val_regex;
				format += ")";
			}
			else
			{
				format += str[i];
			}
		}
		format += "$";
		return {v, std::regex(format)};
	}

	SynchronizedFile logger;
	SynchronizedFile errors;
	SynchronizedFile performancer;
	std::string template_dir;
	jinja2::Template tpl;

	std::map<std::string, responcer> responses;

public:
	webnetpp()
	{
		performancer = SynchronizedFile(std::clog);
		logger = SynchronizedFile(std::clog);
		errors = SynchronizedFile(std::cout);
		template_dir = ".";
	}

	template <typename F>
	webnetpp &handle(std::string code, F _res)
	{
		const auto res = wrap(_res);
		responses[code] = responcer(res);
		return *this;
	}

	template <typename Ret, typename... Ts>
	webnetpp &handle(std::string code, std::function<Ret(Ts...)> const &res)
	{
		responses[code] = responcer(res);
		return *this;
	}

private:
	auto get_routes() const
	{
		return this->routes;
	}

public:
	webnetpp &set_performancer(std::ostream &_performancer = std::clog)
	{
		performancer = SynchronizedFile(_performancer);
		return *this;
	}

	webnetpp &set_logger(std::ostream &_logger = std::clog)
	{
		logger = SynchronizedFile(_logger);
		return *this;
	}

	webnetpp &set_error_logger(std::ostream &_logger = std::clog)
	{
		errors = SynchronizedFile(_logger);
		return *this;
	}

	webnetpp &set_static(std::string path, std::string alias)
	{
		fs::path p = fs::relative(path);
		this->route(alias + "/{path}", [&path, this](std::string file) {
			return this->get_file(path + "/" + file);
		});
		return *this;
	}

	webnetpp &set_templates(std::string path)
	{
		this->template_dir = path;
		return *this;
	}

private:
	response get_file(std::string path) const
	{
		std::string ext = fs::path(path).extension();
		const std::string mime = mime_types::get_mime_type(ext.c_str()).data();
		std::ifstream ifs(path);
		if (not ifs.is_open())
		{
			path_vars p;
			p += path_vars::var(path, "string");
			return this->responses.at("404").call(p);
		}
		else
		{
			std::string content((std::istreambuf_iterator<char>(ifs)),
								(std::istreambuf_iterator<char>()));
			std::map<std::string, std::string> m = {{"Content-type", mime + "; charset=utf-8"}};
			return response(status_line("1.1", "200"), m, content);
		}
	}
public:
	response render(std::string path, jinja2::ValuesMap params = {})
	{
		path = this->template_dir + "/" + path;
		std::string response_str;
		try
		{
			this->tpl.LoadFromFile(path);
			response_str = std::move(tpl.RenderAsString(params).value());
		}
		catch (...)
		{ // file not found
			return this->responses.at("404").call(path_vars() += path_vars::var(path, "string"));
		}
		return response(response_str);
	}

	template <typename Ret, typename... Ts>
	webnetpp &route(std::string path, std::function<Ret(Ts...)> const &res)
	{
		auto x = convert_path_to_regex(path);
		if (routes.find(x) == routes.end())
			routes[x] = responcer(res);
		else // rewriting path
			routes[x] = responcer(res);
		return *this;
	}

	template <typename F>
	webnetpp &route(std::string path, F _res)
	{
		const auto res = wrap(_res);
		auto x = convert_path_to_regex(path);
		if (routes.find(x) == routes.end())
			routes[x] = responcer(res);
		else // rewriting path
			routes[x] = responcer(res);
		return *this;
	}

private:
	response respond(const std::string &path)
	{
		// this->logger << "Requested: " << req.uri << "\n";
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
					return s.second.call(params);
				}
			}
		}
		path_vars p;
		p += path_vars::var(path, "string");
		return responses.at("404").call(p);
	}

	response respond(const request &req)
	{
		return respond(req.uri);
	}

	response respond(const char *p)
	{
		return respond(std::string(p));
	}

	static void accept_req(size_t id, webnetpp *app)
	{
		auto t1 = std::chrono::high_resolution_clock::now();
		int &new_socket = app->clients[id];
		try
		{
			char buffer[8196] = {0};
			recv(new_socket, buffer, 8196, 0);
			// app->logger << "Accepted data:\n" << buffer << "\n--------- END-OF-DATA ------------\n";

			if (strlen(buffer) == 0)
			{
				// app->logger << "================= EMPTY-BUFFER ===============\n";
				app->busy[id] = false;
				return;
			}

			response res;
			request r(buffer);
			// app->logger << "================= PARSING-BUFFER ===============\n";
			// app->logger << r.to_string () << "\n";
			// app->logger << "--------- PARSED ------------\n";

			try
			{
				res = app->respond(r);
			}
			catch (std::exception &e)
			{
				res = app->responses.at("500").call(path_vars() += {std::string(e.what()), "string"});
			}
			res.set_http(r.http);
			std::string s = res.to_string();
			send(new_socket, s.c_str(), s.size(), 0);
			shutdown(new_socket, 2);
			app->busy[id] = false;
			auto t2 = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> elapsed = t2 - t1;
			app->performancer << elapsed.count() << " miliseconds\n";
			return;
		}
		catch (...)
		{
			shutdown(new_socket, 2);
			app->busy[id] = false;
		}
	}

public:
	int run(unsigned short PORT, unsigned int threads, bool limited = false, unsigned requests = -1)
	{
#ifdef __WIN32__
		WSADATA wsa;

		// this->logger << "Initialising Winsock...\n";
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			this->errors << "Failed. Error Code: " << WSAGetLastError() << "\n";
			throw std::runtime_error("Unable to initiate Winsock: " + std::to_string(WSAGetLastError()));
		}

		// this->logger << "Initialised.\n";
#endif

		int server_fd;
		struct sockaddr_in address;
		int addrlen = sizeof(address);

		// Creating socket file descriptor
		if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		{
			this->errors << "Failed. Unable to initiate socket." << "\n";
			throw std::runtime_error("Unable to initiate socket.");
		}

		// Forcefully attaching socket to the port
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons(PORT);

		// Forcefully attaching socket to the port
		if (bind(server_fd, (struct sockaddr *)&address,
				 sizeof(address)) < 0)
		{
			this->errors << "Socket binding failed." << "\n";
			throw std::runtime_error("Unable to bind with the socket.");
		}
		if (listen(server_fd, 3) < 0)
		{
			this->errors << "Socket listening failed." << "\n";
			throw std::runtime_error("Unable to listen to the socket.");
		}

		// this->logger << "Listening\n";
		//std::cout << "Listening" << std::endl;

		//std::cout << "Starting initing the threads" << std::endl;
		threads = std::min(threads, requests);
		threads_ptr = new mingw_stdthread::thread *[threads];
		busy = new bool[threads];
		clients = new int[threads];
		for (size_t i = 0; i < threads; i++)
		{
			threads_ptr[i] = nullptr;
			busy[i] = false;
		}
		while (requests >= 1 || !limited)
		{
			bool is_there_thread = false;
			size_t i = 0;
			while (!is_there_thread)
				for (i = 0; i < threads; i++)
					if (threads_ptr[i] == nullptr)
					{
						busy[i] = true;
						is_there_thread = true;
						break;
					}
					else if (busy[i] == false)
					{
						delete threads_ptr[i];
						busy[i] = true;
						is_there_thread = true;
						break;
					}
			if ((clients[i] = accept(server_fd, (struct sockaddr *)&address,
									 (socklen_t *)&addrlen)) < 0)
			{
				// this->errors << "Failed to connect to client\n";
				continue;
			}

			//std::cout << "New thread creating" << std::endl;
			threads_ptr[i] = new mingw_stdthread::thread(accept_req, i, this);
			//std::cout << "Detaching the new thread" << std::endl;
			threads_ptr[i]->join();
			//std::cout << "Successfully detached the new thread" << std::endl;
			requests--;
		}
		for (size_t i = 0; i < threads; i++)
		{
			while (busy[i])
			{
			}
		}
		return true;
	}

private:
	mingw_stdthread::thread **threads_ptr;
	bool *busy;
	int *clients;
};
} // namespace webnetpp
