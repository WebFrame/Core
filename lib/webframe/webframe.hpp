/** 
 *  @file   webframe.hpp 
 *  @brief  Single header containing all the necessary tools regarding WebFrame 
 *  @author Alex Tsvetanov
 *  @date   2022-03-07
 ***********************************************/

#pragma once

namespace webframe
{
	enum class method
	{
		undefined,
		GET,
		HEAD,
		POST,
		PUT,
		DDELETE,
		CONNECT,
		OPTIONS,
		TRACE,
		PATCH
	};

	enum class LoadingState 
	{
		NOT_STARTED = -1,
		METHOD = 0,
		URI = 1,
		PARAM_KEY = 2,
		PARAM_VALUE = 3,
		HTTP_IN_PROGRESS = 4,
		HTTP_LOADED = 5,
		HEADER_ROW = 6,
		BODY = 7,
		LOADED = 8
	};
	
	class webframe;
	struct path_vars;
	class response;
	struct status_line;
	class request;
	
	constexpr const char* method_to_string (const method& m);
	constexpr method string_to_method (const char* m);
}

#include <webframe/base.hpp>
#include <webframe/file.hpp>
#include <webframe/mime.hpp>
#include <webframe/lambda2function.hpp>
#include <webframe/respond_manager.hpp>
#include <webframe/webframe-impl.hpp>