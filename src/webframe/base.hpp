/** 
 *  @file   base.hpp 
 *  @brief  Basic utils to handle web requests and reponses
 *  @author Alex Tsvetanov
 *  @date   2022-03-07
 ***********************************************/

#pragma once

#include <map>
#include <string>
#include <iostream>
#include <regex>
#include <any>
#include <stdexcept>
#include <utility>
#include <webframe/http_codes.hpp>
#include <webframe/mime.hpp>

namespace webframe
{
	const std::string end_line = "\n";

	/** 
	 *  @brief   Type of request prameters
	 *  @details This is the type of all parameters passed as request paramertes
	 *  @note    Ex. In 'url?param=value' param and value will be saved in the map as key and value.
	 ***********************************************/
	using req_vars = std::map < std::string, std::string >;
	
	/** 
	 *  @brief   Type of path prameters
	 *  @details This is the type of all variables passed as part of the unique URL
	 *  @note    Ex. In 'url/variable' the value of the variable will be saved in the variable.
	 ***********************************************/
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

	/** 
	 *  @brief   Type of the status line of the response
	 *  @details This type represents the HTTP version and the HTTP code of the reponse
	 ***********************************************/
	struct status_line
	{
	public:
	private:
		std::string http;
		std::string code;
		std::string output;

		void rebuild_string () {
			output = "HTTP/" + this->http + " " + this->code + " " + std::string(http_codes::get_reason_by_code(this->code.c_str())) + end_line;
		}
	public:
		status_line (std::string _code) : http ("1.1"), code (_code) 
		{
			rebuild_string();
		}
		
		status_line (std::string _http, std::string _code) : http (_http), code (_code) 
		{
			rebuild_string();
		}

		const std::string& to_string () const
		{
			return output;
		}

		friend response;
	};

	/** 
	 *  @brief   Type of the response
	 *  @details This type represents the status line, all headers, and the body of the response
	 ***********************************************/
	class response 
	{
		private:
			status_line status;
			std::map < std::string, std::string > header;
			std::stringbuf body;
			std::stringstream res;
		public:
			response (const std::string& html): 
				response(status_line ("1.1", "200"), {{"Content-type", "text/html"}}, html)
			{
				rebuild_string();
			}
			response (const std::string& http, const std::string& html): 
				response(status_line (http, "200"), {{"Content-type", "text/html"}}, html)
			{
				rebuild_string();
			}
			response (status_line status, const std::string& html): 
				response(status, {{"Content-type", "text/html"}}, html)
			{
				rebuild_string();
			}
			response (status_line s = status_line ("2.0", "204"), const std::map < std::string, std::string >& m = {}, const std::string& _body = ""): 
				status (s), header (m), body (_body, std::ios::in)
			{
				rebuild_string();
			}
			response (const std::string& http, const response& r) :
				status(http, r.status.code), header (r.header), body(r.body.str())
			{
				rebuild_string();
			}
		private:
			void rebuild_string ()
			{
				res << status.to_string();
				for (auto& x : header)
				{
					res << x.first + ": " + x.second + end_line;
				}
				res << end_line;
				res << body.str();
			}
		public:
			const std::stringstream& to_string() const 
			{
				return res;
			}
	};
	
	/** 
	 *  @brief    Casts webframe::method to const char*
	 *  @see      webframe::method
	 ***********************************************/
	constexpr const char* method_to_string (const method& m)
	{
		return
			(m == method::GET    ) ? "GET" :
			(m == method::HEAD   ) ? "HEAD" :
			(m == method::POST   ) ? "POST" :
			(m == method::PUT    ) ? "PUT" :
			(m == method::DDELETE) ? "DELETE" :
			(m == method::CONNECT) ? "CONNECT" :
			(m == method::OPTIONS) ? "OPTIONS" :
			(m == method::TRACE  ) ? "TRACE" :
			(m == method::PATCH  ) ? "PATCH" : 
			([]() -> const char* {
				throw std::invalid_argument ("Not valid METHOD Type");
			})();
	}
	
	/** 
	 *  @brief    Casts const char* to webframe::method
	 *  @see      webframe::method
	 ***********************************************/
	constexpr method string_to_method (const char* m)
	{
		return (strcmp(m, "GET") == 0) ?     method::GET :
		       (strcmp(m, "HEAD") == 0) ?    method::HEAD :
		       (strcmp(m, "POST") == 0) ?    method::POST :
		       (strcmp(m, "PUT") == 0) ?     method::PUT :
		       (strcmp(m, "DELETE") == 0) ?  method::DDELETE :
		       (strcmp(m, "CONNECT") == 0) ? method::CONNECT :
		       (strcmp(m, "OPTIONS") == 0) ? method::OPTIONS :
		       (strcmp(m, "TRACE") == 0) ?   method::TRACE :
		       (strcmp(m, "PATCH") == 0) ?   method::PATCH :
		       throw std::invalid_argument (std::string(m) + " is not a valid METHOD Type");
	}
	
	/** 
	 *  @brief   Type of the request
	 *  @details This type represents the HTTP version, the URL, the URL variables, all headers, and the body of the request
	 *  @see     webframe::path_vars
	 ***********************************************/
	class request
	{
		private:
			LoadingState loading;
			std::string remaining_to_parse;
		public:
			method m;
			std::string uri;
			req_vars request_params;
			std::string http;
			std::map < std::string, std::string > header;
			std::string body;
			std::string output;

			void rebuild_string() 
			{
				output = std::string(method_to_string(m)) + " " + uri + " HTTP/" + http + end_line;
				for (auto& x : request_params)
					output += x.first + ": " + x.second + end_line;
				for (auto& x : header)
					output += x.first + ": " + x.second + end_line;
				output += end_line;
				output += body;
			}
		public:
			request () {
				loading = LoadingState::NOT_STARTED;
				remaining_to_parse = "";
			}

			request (method _m, std::string h, std::map < std::string, std::string > m, std::string _body) : m (_m), http (h), header (m), body (_body)
			{}

			request (char* buff) : request() {
				this->loadMore(buff, strlen(buff));
			}

			void loadMore(char* buff, size_t n)
			{
				if (n != 0)
				{
					size_t i = 0;
					if(loading == LoadingState::NOT_STARTED) 
					{
						for ( ; i < n ; i ++)
						{
							if (buff [i] == ' ')
							{
								m = string_to_method (remaining_to_parse.c_str());
								loading = LoadingState::METHOD;
								remaining_to_parse = "";
								i ++;
								break;
							}
							remaining_to_parse += buff [i];
						}
					}
					if(loading == LoadingState::METHOD)
					{
						for ( ; i < n ; i ++)
						{
							if (buff [i] == ' ') {
								uri = remaining_to_parse;
								loading = LoadingState::HTTP_IN_PROGRESS;
								remaining_to_parse = "";
								i ++;
								break;
							}
							if (buff [i] == '?') {
								uri = remaining_to_parse;
								loading = LoadingState::PARAM_KEY;
								remaining_to_parse = "";
								i ++;
								break;
							}
							remaining_to_parse += buff [i];
						}
					}
					if(loading == LoadingState::PARAM_KEY or
					   loading == LoadingState::PARAM_VALUE)
					   {
						std::string var = "", val = "";
						std::string* x;
						if (loading == LoadingState::PARAM_VALUE) x = &var;
						if (loading == LoadingState::PARAM_KEY) x = &val;

						(*x) = remaining_to_parse;

						for ( ; i < n ; i ++)
						{
							if (buff [i] == '=')
							{
								loading = LoadingState::PARAM_KEY;
								remaining_to_parse = "";
								x = &val;
								continue;
							}
							if (buff [i] == '&' or buff [i] == ' ')
							{
								loading = LoadingState::PARAM_VALUE;
								remaining_to_parse = "";
								request_params[var] = val;
								var = val = "";
								x = &var;
							}
							if (buff [i] == ' ') {
								loading = LoadingState::HTTP_IN_PROGRESS;
								remaining_to_parse = "";
								break;
							}
							(*x) += buff [i];
							remaining_to_parse += buff [i];
						}
					}
					if(loading == LoadingState::HTTP_IN_PROGRESS)
					{
						// skip " HTTP/" to reach "1.1"
						for ( ; i < n and !(buff[i] >= '0' and buff[i] <= '9') and buff [i] != '.' ; i ++){}
						// read version
						for ( ; i < n ; i ++)
						{
							if (buff [i] == '\r') {
								http = remaining_to_parse;
								loading = LoadingState::HTTP_LOADED;
								remaining_to_parse = "";
								i ++;
								break;
							}
							remaining_to_parse += buff [i];
						}
					}
					if(loading == LoadingState::HTTP_LOADED)
					{
						for ( ; i < n ; i ++)
						{
							loading = LoadingState::HEADER_ROW;
							for ( ; i < n ; i ++)
							{
								if (buff [i] == '\r')
								{
									i ++;
									break;
								}
								if (buff [i] != ' ')
									remaining_to_parse += buff [i];
							}
							if (remaining_to_parse == "") {
								loading = LoadingState::BODY;
								break;
							}
							header [remaining_to_parse.substr (0, remaining_to_parse.find (':'))] = remaining_to_parse.substr (remaining_to_parse.find (':') + 1);
							remaining_to_parse = "";
						}
					}
					if(loading == LoadingState::BODY)
					{
						body = remaining_to_parse + std::string((char*)(buff+i));
					}
				}
			}

			void finalize () 
			{
				loading = LoadingState::LOADED;
				rebuild_string();
			}

			const std::string& to_string () const
			{
				return output;
			}
	};
}