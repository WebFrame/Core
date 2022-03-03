COMPILER_CPP=g++
CPP_STD=-std=c++2a
OPT=-O3
INCLUDE_DIRS=-I./lib/boost -I./tests -I./src
LIB_FLAGS=-static -pthread -lboost_system -lpthread -fconcepts
INJACPP=-I./lib/inja/single_include/ -I./lib/inja/include/inja/third_party/include

ifeq ($(OS),Windows_NT)     # is Windows_NT on XP, 2000, 7, Vista, 10...
	LIB_FLAGS += -lwsock32 -lws2_32
else
	LIB_FLAGS += -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
endif

WARNING_FLAGS=-Wall -pedantic

all: clean build build_test run_tests

install: install_inja install_boost

install_boost:
	cd ./lib/boost && ./bootstrap.sh && ./b2 headers

install_inja:
	cd ./lib/inja && cmake . -G "Unix Makefiles" && make

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
	git clone https://github.com/danmar/cppcheck; mkdir build; cd build; cmake ..; cmake --build .; cd ..; echo "" > cppcheck_report.txt; cppcheck $(INCLUDE_DIRS) $(LIB_FLAGS) $(JINJA2CPP) $(JINJA2CPP_DEFINES) --std=c++17 --config-exclude=./lib ./src/ --xml --enable=information 2>> check_report.xml; ./cppcheck/htmlreport/cppcheck-htmlreport --source-dir ./ --report-dir=./html_report --file=cppcheck_report.xml
        