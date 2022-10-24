cmake_minimum_required(VERSION 3.14)

include(CTest)
enable_testing()

find_package(Threads REQUIRED)

add_executable(Core-Tests main.cpp)
target_include_directories(Core-Tests PUBLIC "${PROJECT_SOURCE_DIR}/tests")
target_link_libraries(Core-Tests PUBLIC Core pthread)

add_test(NAME Usage COMMAND Core-Tests)
set_tests_properties(Usage
  PROPERTIES 
  PASS_REGULAR_EXPRESSION "+ [0-9]+)"
  FAIL_REGULAR_EXPRESSION "x [0-9]+)"
)
