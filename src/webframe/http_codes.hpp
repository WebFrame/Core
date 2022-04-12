/** 
 *  @file   http_codes.hpp 
 *  @brief  Utils for all possible HTTP response codes
 *  @author Alex Tsvetanov
 *  @date   2022-03-07
 ***********************************************/

#pragma once

#include <stdlib.h>
#include <string_view>
#include <webframe/constexpr.hpp>

namespace http_codes {
    /** 
     *  @brief   List of all HTTP response messages (grouped by first digit of the code)
     ***********************************************/
    constexpr const char* codes [6][18] = {
        /*0xx errors*/{},
        /*1xx errors*/{"Continue", "Switching Protocols"},
        /*2xx errors*/{"OK", "Created", "Accepted", "Non-Authoritative Information", "No Content", "Reset Content", "Partial Content"},
        /*3xx errors*/{"Multiple Choices", "Moved Permanently", "Found", "See Other", "Not Modified", "Use Proxy", "Temporary Redirect"},
        /*4xx errors*/{"Bad Request", "Unauthorized", "Payment Required", "Forbidden", "Not Found", "Method Not Allowed", "Not Acceptable", "Proxy Authentication Required", "Request Time-out", "Conflict", "Gone", "Length Required", "Precondition Failed", "Request Entity Too Large", "Request-URI Too Large", "Unsupported Media Type", "Requested range not satisfiable", "Expectation Failed"},
        /*5xx errors*/{"Internal Server Error", "Not Implemented", "Bad Gateway", "Service Unavailable", "Gateway Time-out", "HTTP Version not supported"},
    };

    /** 
     *  @brief   Transforms HTTP code into HTTP response message
     *  @param   code The HTTP code which's message is requested as integer
     ***********************************************/
    constexpr const char* get_reason_by_code(unsigned int code) {
        return codes[code / 100][code % 100];
    }

    /** 
     *  @brief   Transforms HTTP code to HTTP response message
     *  @param   code_str The HTTP code which's message is requested as a string
     ***********************************************/
    constexpr const char* get_reason_by_code(const char* code_str) {
        return get_reason_by_code(webframe::_compile_time::string_to_uint(code_str));
    }
};