#pragma once

#include <stdlib.h>
#include <string_view>

namespace http_codes {
    constexpr std::string_view codes [6][18] = {
        /*0xx errors*/{},
        /*1xx errors*/{"Continue", "Switching Protocols"},
        /*2xx errors*/{"OK", "Created", "Accepted", "Non-Authoritative Information", "No Content", "Reset Content", "Partial Content"},
        /*3xx errors*/{"Multiple Choices", "Moved Permanently", "Found", "See Other", "Not Modified", "Use Proxy", "Temporary Redirect"},
        /*4xx errors*/{"Bad Request", "Unauthorized", "Payment Required", "Forbidden", "Not Found", "Method Not Allowed", "Not Acceptable", "Proxy Authentication Required", "Request Time-out", "Conflict", "Gone", "Length Required", "Precondition Failed", "Request Entity Too Large", "Request-URI Too Large", "Unsupported Media Type", "Requested range not satisfiable", "Expectation Failed"},
        /*5xx errors*/{"Internal Server Error", "Not Implemented", "Bad Gateway", "Service Unavailable", "Gateway Time-out", "HTTP Version not supported"},
    };
    constexpr unsigned int string_to_uint(const char* str, unsigned int value = 0) {
        return (*str) == '\0' ? value : string_to_uint(str + 1, value * 10 + ((*str) - '0'));
    }
    constexpr std::string_view get_reason_by_code(unsigned int code) {
        return codes[code / 100][code % 100];
    }
    constexpr std::string_view get_reason_by_code(const char* code_str) {
        return codes[string_to_uint(code_str) / 100][string_to_uint(code_str) % 100];
    }
};