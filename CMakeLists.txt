cmake_minimum_required(VERSION 3.0.0)
project(WebFrame VERSION 2.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

include(CTest)
enable_testing()

add_library(WebFrame src/webframe/webframe.hpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
