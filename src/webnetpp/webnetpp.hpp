#pragma once

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

//#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
namespace this_coro = boost::asio::this_coro;

#if defined(BOOST_ASIO_ENABLE_HANDLER_TRACKING)
# define use_awaitable \
  boost::asio::use_awaitable_t(__FILE__, __LINE__, __PRETTY_FUNCTION__)
#endif

#include <webnetpp/base.hpp>
#include <webnetpp/file.hpp>
#include <webnetpp/mime.hpp>
#include <webnetpp/lambda2function.hpp>

#include <inja/inja.hpp>

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
		responser,
		cmp>
		routes;

	static std::pair<std::vector<std::string>, // var type
					 std::regex>			   // regex
	convert_path_to_regex(std::string str, webnetpp& app)
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
		responses[code] = responser(res);
		return *this;
	}

	template <typename Ret, typename... Ts>
	webnetpp &handle(std::string code, std::function<Ret(Ts...)> const &res)
	{
		responses[code] = responser(res);
		return *this;
	}

	auto get_routes() const
	{
		return this->routes;
	}

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
		std::filesystem::path p = std::filesystem::relative(path);
		this->route(alias + "/{path}", [&path, this](std::string file) {
			std::cout << "Requested file: " << path + "/" + file << std::endl;
			return this->get_file(path + "/" + file);
		});
		return *this;
	}

	webnetpp &set_templates(std::string path)
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
	webnetpp &route(std::string path, std::function<Ret(Ts...)> const &res)
	{
		auto x = convert_path_to_regex(path, *this);
		if (routes.find(x) == routes.end())
			routes[x] = responser(res);
		else // rewriting path
			routes[x] = responser(res);
		return *this;
	}

	template <typename F>
	webnetpp &route(std::string path, F _res)
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

	response respond(const request &req, const std::string& http = "1.1")
	{
		return respond(req.uri, http);
	}

	response respond(const char *p, const std::string& http = "1.1")
	{
		return respond(std::string(p), http);
	}

private:

	awaitable<void> responder(tcp::socket socket, std::function<void()> callback)
	{
		auto t1 = std::chrono::high_resolution_clock::now();
		try
		{
			char data[2048];
			std::size_t n = -1;
			request r;
			for (; n >= sizeof(data)/sizeof(char) ;)
			{
				n = co_await socket.async_read_some(boost::asio::buffer(data), use_awaitable);
				r.loadMore(data, n);
			}
			r.finalize();
			{
				response res;
				try
				{
					res = this->respond(r, r.http);
				}
				catch (std::exception &e)
				{
					res = this->responses.at("500").call(r.http, path_vars() += {std::string(e.what()), "string"});
				}
				std::string response = res.to_string().str();
				size_t responseSize = response.size();
				co_await async_write(socket, boost::asio::buffer(std::move(response), responseSize), use_awaitable);
				auto t2 = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double, std::milli> elapsed = t2 - t1;
				this->performancer << r.uri << " " << elapsed.count() << "\n";
				socket.shutdown(boost::asio::socket_base::shutdown_both);
			}
		}
		catch (std::exception& e)
		{
			this->logger << "echo Exception: " << e.what() << "\n";
		}
		if (callback)
			callback();
	}

	awaitable<void> listener(const unsigned short int PORT, std::function<void()> callback)
	{
		auto executor = co_await this_coro::executor;
		tcp::acceptor acceptor(executor, {tcp::v4(), PORT});
		for (;;)
		{
			tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
			co_spawn(executor, responder(std::move(socket), callback), detached);
		}
	}
public:
	bool run(unsigned short PORT, unsigned int threads, bool limited = false, unsigned int requests = -1) {
		try
		{
			boost::asio::io_context io_context(threads);

			boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
			signals.async_wait([&](auto, auto){ io_context.stop(); });

			if (limited)
				co_spawn(io_context, listener(PORT, [&requests, limited, &io_context]() {
					requests--;
					if (limited && requests == 0)
						io_context.stop();
				}), detached);
			else
				co_spawn(io_context, listener(PORT, std::function<void()>()), detached);

			io_context.run();
			return true;
		}
		catch (std::exception& e)
		{
			this->logger << "Exception: " << e.what() << "\n";
			return false;
		}
	}
private:
	std::thread **threads_ptr;
	std::mutex* busy;
	int* clients;
};
} // namespace webnetpp
