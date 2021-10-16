COMPILER_CPP=g++
CPP_STD=--std=c++17
OPT=-O3
INCLUDE_DIRS=-I./tests -I./src -I./lib/win/mingw-std-threads-master -I./lib/win/filesystem-master/include/ -I./lib/win/stopwatch/ 
JINJA2CPP=-I./lib/Jinja2Cpp/.jinja2cpp-install/include -L./lib/Jinja2Cpp/.jinja2cpp-install/lib/static -ljinja2cpp
JINJA2CPP_DEFINES=-Dvariant_CONFIG_SELECT_VARIANT=variant_VARIANT_NONSTD -Dnssv_CONFIG_SELECT_STRING_VIEW=nssv_STRING_VIEW_NONSTD -Doptional_CONFIG_SELECT_OPTIONAL=optional_OPTIONAL_NONSTD -DBOOST_SYSTEM_NO_DEPRECATED -DBOOST_ERROR_CODE_HEADER_ONLY
LIB_FLAGS=-static

ifeq ($(OS),Windows_NT)     # is Windows_NT on XP, 2000, 7, Vista, 10...
    LIB_FLAGS += -lwsock32 -lws2_32
else
	LIB_FLAGS += -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
endif

WARNING_FLAGS=-Wall -Wno-long-long -pedantic -fconcepts

all: build build_test run_tests clean

install:
	cd ./lib && rm -rf Jinja2Cpp && git clone https://github.com/jinja2cpp/Jinja2Cpp && cd Jinja2Cpp && mkdir .jinja2cpp-build && cd .jinja2cpp-build && cmake ../ -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../.jinja2cpp-install -DJINJA2CPP_BUILD_TESTS=OFF -DJINJA2CPP_DEPS_MODE=internal -DJINJA2CPP_CXX_STANDARD=17 && cmake --build . --config Release --target install

build:
	$(COMPILER_CPP) $(CPP_STD) $(OPT) ./src/main.cpp -o ./bin/main.exe $(INCLUDE_DIRS) $(WARNING_FLAGS) $(LIB_FLAGS) $(JINJA2CPP) $(JINJA2CPP_DEFINES)

build_test:
	$(COMPILER_CPP) $(CPP_STD) $(OPT) ./tests/main.cpp -o ./bin/test.exe $(INCLUDE_DIRS) $(WARNING_FLAGS) $(LIB_FLAGS) $(JINJA2CPP) $(JINJA2CPP_DEFINES)

run:
	./bin/main.exe

run_tests:
	mkdir -p ./bin/log
	./bin/test.exe
	cat ./bin/log/performance.txt

clean:
	rm -rf ./bin/log/*

cppcheck:
	git clone https://github.com/danmar/cppcheck
	mkdir build
	cd build
	cmake ..
	cmake --build .
	cd ..
	echo "" > cppcheck_report.txt
	cppcheck -I./tests -I./src -I./lib/win/mingw-std-threads-master -I./lib/win/filesystem-master/include/ -I./lib/win/stopwatch/ --std=c++17 --config-exclude=./lib ./src/ --xml --enable=information 2>> check_report.xml
	./cppcheck/htmlreport/cppcheck-htmlreport --source-dir ./ --report-dir=./html_report --file=cppcheck_report.xml
        