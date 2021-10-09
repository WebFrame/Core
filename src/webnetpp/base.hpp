#pragma once

#include <map>
#include <string>
#include <iostream>
#include <regex>
#include <any>
#include <stdexcept>
#include <utility>
#include <webnetpp/http_codes.hpp>
#include <webnetpp/mime.hpp>

namespace webnetpp
{
	const std::string end_line = /*\r*/"\n";
	
	struct path_vars 
	{
			struct var 
			{
				var () 
				{
					this->type = "string";
					this->value = "";
				}
				std::string type;
				std::string value;
				var (std::string value, std::string type = "string")
				{
					this->type = type;
					this->value = value;
				}

				explicit operator int () const
				{
					if (value.size () == 0)
						throw std::invalid_argument("path_vars::var::value is empty.");
					int ans = 0;
					if (value[0] == '-')
						ans = -(value[1] - '0');
					for (size_t i = (value[0] == '-') ; i < value.size () ; i ++)
						if (value[i] >= '0' and value[i] <= '9')
							ans = ans * 10 + value[i] - '0';
						else
							throw std::invalid_argument("path_vars::var::value is not matching path_vars::var::type (not integer)"); 
					return ans;
				}
				explicit operator const char* () const
				{
					if (value.size () == 0)
						throw std::invalid_argument("path_vars::var::value is empty.");
					return value.c_str();
				}
				explicit operator char () const
				{
					if (value.size () == 0)
						throw std::invalid_argument("path_vars::var::value is empty.");
					if (value.size () != 1)
						throw std::invalid_argument("path_vars::var::value is too long.");
					return value[0];
				}
				explicit operator std::string () const
				{
					if (value.size () == 0)
						throw std::invalid_argument("path_vars::var::value is empty.");
					return value;
				}
				explicit operator std::string& () const
				{
					if (value.size () == 0)
						throw std::invalid_argument("path_vars::var::value is empty.");
					std::string* ans = new std::string(value);
					return *ans;
				}
				template<typename T>
    			explicit operator T&() const { 
					T* ans = new T(value);
					return *ans;
				}
				template<typename T>
    			explicit operator T() const { 
					T* ans = new T(value);
					return *ans;
				}
			};
			path_vars () {}
		private:
			std::vector < var > vars;
		public:
			const var operator[] (long long unsigned int ind) const
			{
				return vars[ind];
			}
			path_vars& operator += (const var& v)
			{
				vars.push_back (v);
				return *this;
			}
	};

	struct status_line
	{
	public:
	private:
		std::string http;
		std::string code;
	public:
		status_line (std::string _code) : http (""), code (_code) 
		{}
		
		status_line (std::string _http, std::string _code) : http (_http), code (_code) 
		{}

		status_line& set_http (std::string h)
		{
			this->http = h;
			return *this;
		}

		std::string to_string () const
		{
			return "HTTP/" + this->http + " " + this->code + " " + std::string(http_codes::get_reason_by_code(this->code.c_str())) + end_line;
		}
	};

	class response 
	{
		private:
			status_line status;
			std::map < std::string, std::string > header;
			std::string body;
		public:
			response (std::string html): 
				response(status_line ("1.1", "200"), {{"Content-type", "text/html"}}, html)
			{}
			response (std::string http, std::string html): 
				response(status_line (http, "200"), {{"Content-type", "text/html"}}, html)
			{}
			response (status_line status, std::string html): 
				response(status, {{"Content-type", "text/html"}}, html)
			{}
			response (status_line s = status_line ("1.1", "204"), std::map < std::string, std::string > m = {}, std::string _body = ""): 
				status (s), header (m), body (_body)
			{}

			std::string to_string () const
			{
				std::string res = status.to_string ();
				for (auto& x : header)
					res += x.first + ": " + x.second + end_line;
				res += end_line;
				res += body;
				return res;
			}

			response& set_http (std::string h)
			{
				status.set_http (h);
				return *this;
			}
	};

	enum class method
	{
		GET
		,HEAD
		,POST
		,PUT
		,DDELETE
		,CONNECT
		,OPTIONS
		,TRACE
		,PATCH
	};

	std::string to_string (const method& m)
	{
		switch (m)
		{
			case method::GET: return "GET";
			case method::HEAD: return "HEAD";
			case method::POST: return "POST";
			case method::PUT: return "PUT";
			case method::DDELETE: return "DELETE";
			case method::CONNECT: return "CONNECT";
			case method::OPTIONS: return "OPTIONS";
			case method::TRACE: return "TRACE";
			case method::PATCH: return "PATCH";
			default: throw std::invalid_argument ("Not valid METHOD Type");
		}
	}

	method to_method (const std::string& m)
	{
		if (m == "GET") return method::GET;
		if (m == "HEAD") return method::HEAD;
		if (m == "POST") return method::POST;
		if (m == "PUT") return method::PUT;
		if (m == "DELETE") return method::DDELETE;
		if (m == "CONNECT") return method::CONNECT;
		if (m == "OPTIONS") return method::OPTIONS;
		if (m == "TRACE") return method::TRACE;
		if (m == "PATCH") return method::PATCH;
		throw std::invalid_argument (m + " is not a valid METHOD Type");
	}
	
	using req_vars = std::map < std::string, std::string >;
	
	class request
	{
		private:
		public:
			method m;
			std::string uri;
			req_vars request_params;
			std::string http;
			std::map < std::string, std::string > header;
			std::string body;
		public:
			request (method _m, std::string h, std::map < std::string, std::string > m, std::string _body) : m (_m), http (h), header (m), body (_body)
			{}

			request (char* buff)
			{
				if (strlen (buff) != 0)
				{
					std::string M;
					size_t i;
					for (i = 0 ; i < strlen (buff) ; i ++)
					{
						if (buff [i] == ' ')
							break;
						M += buff [i];
					}
					m = to_method (M);
					for (i ++ ; i < strlen (buff) ; i ++)
					{
						if (buff [i] == ' ' or buff [i] == '?')
							break;
						uri += buff [i];
					}
					if (buff [i] == '?')
					{
						std::string var = "", val = "";
						std::string* x = &var;

						for (i ++ ; i < strlen (buff) ; i ++)
						{
							//std::cout << (*x) << "; ;" << buff [i] << std::endl;
							//std::cout << "\t" << var << "; ;" << val << std::endl;
							if (buff [i] == '=')
							{
								x = &val;
								continue;
							}
							if (buff [i] == '&' or buff [i] == ' ')
							{
								request_params[var] = val;
								var = val = "";
								x = &var;
							}
							if (buff [i] == ' ')
								break;
							(*x) += buff [i];
						}
					}
					http = "";
					for (i += 6 ; i < strlen (buff) ; i ++)
					{
						if (buff [i] == '\r')
							break;
						http += buff [i];
					}
					for (i ++ ; i < strlen (buff) ; i ++)
					{
						std::string line = "";
						for ( ; i < strlen (buff) ; i ++)
						{
							if (buff [i] == '\r')
							{
								i ++;
								break;
							}
							if (buff [i] != ' ')
								line += buff [i];
						}
						if (line == "") break;
						header [line.substr (0, line.find (':'))] = line.substr (line.find (':') + 1);
					}
				}
			}

			std::string to_string () const
			{
				std::string res = webnetpp::to_string(m) + " " + uri + " HTTP/" + http + end_line;
				for (auto& x : request_params)
					res += x.first + ": " + x.second + end_line;
				for (auto& x : header)
					res += x.first + ": " + x.second + end_line;
				res += end_line;
				res += body;
				return res;
			}
	};
}
