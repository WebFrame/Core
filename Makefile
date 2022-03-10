COMPILER_CPP=g++
CPP_STD=-std=c++2a
OPT=-Ofast -fconstexpr-depth=700
INCLUDE_DIRS=-I./lib/asio/asio/include -I./tests -I./src
LIB_FLAGS=-static -pthread -lpthread -fconcepts
INJACPP=-I./lib/inja/single_include/ -I./lib/inja/third_party/include

ifeq ($(OS),Windows_NT)     # is Windows_NT on XP, 2000, 7, Vista, 10...
	LIB_FLAGS += -lwsock32 -lws2_32
else
	LIB_FLAGS += -Wl,--whole-archive -Wl,--no-whole-archive
endif

WARNING_FLAGS=-Wall -Wextra -pedantic

all: build run

test: clean build_test run_tests

install: install_inja install_boost

install_boost:
	cd ./lib/boost && ./bootstrap.sh && ./b2 headers

install_inja:
	cd ./lib/inja && cmake . -G "Unix Makefiles" && make MAKE=make CMAKE_COMMAND=cmake

build:
	$(COMPILER_CPP) $(CPP_STD) $(OPT) ./src/main.cpp -o ./bin/main.exe $(INCLUDE_DIRS) $(WARNING_FLAGS) $(LIB_FLAGS) $(INJACPP)

build_test:
	$(COMPILER_CPP) $(CPP_STD) $(OPT) ./tests/main.cpp -o ./bin/test.exe $(INCLUDE_DIRS) $(WARNING_FLAGS) $(LIB_FLAGS) $(INJACPP)

run:
	./bin/main.exe

run_tests:
	mkdir -p ./bin/log
	./bin/test.exe
	cat ./bin/log/performance_summary.txt

clean:
	rm -rf ./bin
	mkdir -p ./bin

cppcheck:
	cd cppcheck; mkdir build; cd build; cmake ..; cmake --build .; cd ../..; echo "" > check_report.xml; cppcheck/build/bin/Debug/cppcheck.exe -I./tests -I./src --std=c++20 --config-exclude=./lib ./src/ --xml --enable=information 2> check_report.xml; ./cppcheck/htmlreport/cppcheck-htmlreport --source-dir ./ --report-dir=./html_report --file=check_report.xml; mv html_report cppcheck_report
