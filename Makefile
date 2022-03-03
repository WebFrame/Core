COMPILER_CPP=g++
CPP_STD=-std=c++2a
OPT=-O3
INCLUDE_DIRS=-I./lib/boost -I./tests -I./src
# JINJA2CPP=-I./lib/Jinja2Cpp/include -I./lib/Jinja2Cpp/.jinja2cpp-install/include -L./lib/Jinja2Cpp/.jinja2cpp-install/lib/static -ljinja2cpp
# JINJA2CPP_DEFINES=-Dvariant_CONFIG_SELECT_VARIANT=variant_VARIANT_NONSTD -Dnssv_CONFIG_SELECT_STRING_VIEW=nssv_STRING_VIEW_NONSTD -Doptional_CONFIG_SELECT_OPTIONAL=optional_OPTIONAL_NONSTD -DBOOST_SYSTEM_NO_DEPRECATED -DBOOST_ERROR_CODE_HEADER_ONLY
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

install_jinja:
	cd ./lib && cd Jinja2Cpp && mkdir .jinja2cpp-build && cd .jinja2cpp-build && cmake ../ -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../.jinja2cpp-install -DJINJA2CPP_BUILD_TESTS=OFF -DJINJA2CPP_DEPS_MODE=internal -DJINJA2CPP_CXX_STANDARD=17 && cmake --build . --config Release --target install

install_boost:
	cd ./lib && cd boost && ./bootstrap.sh && ./b2 headers

install_inja:
	cd ./lib && rm -rf inja && git clone https://github.com/pantor/inja && cd inja && cmake . -G "Unix Makefiles" && make

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
	git clone https://github.com/danmar/cppcheck
	mkdir build
	cd build
	cmake ..
	cmake --build .
	cd ..
	echo "" > cppcheck_report.txt
	cppcheck $(INCLUDE_DIRS) $(LIB_FLAGS) $(JINJA2CPP) $(JINJA2CPP_DEFINES) --std=c++17 --config-exclude=./lib ./src/ --xml --enable=information 2>> check_report.xml
	./cppcheck/htmlreport/cppcheck-htmlreport --source-dir ./ --report-dir=./html_report --file=cppcheck_report.xml
        