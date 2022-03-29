COMPILER_CPP=g++
CPP_STD=-std=c++2a
OPTIMIZATION_LEVEL=-O2
OPT=-fconstexpr-depth=700
INCLUDE_DIRS=-I./tests -I./src
LIB_FLAGS=-pthread -lpthread
DEBUG_FLAGS=-fsanitize=undefined
INJACPP=-I./lib/inja/single_include/ -I./lib/inja/third_party/include

ifeq ($(OS),Windows_NT)     # is Windows_NT on XP, 2000, 7, Vista, 10...
	LIB_FLAGS += -lwsock32 -lws2_32
else
    ifeq ($(UNAME_S),Darwin)
        LIB_FLAGS += -stdlib=libc++
    endif
endif

WARNING_FLAGS=-Wall -Wextra -pedantic

all: build run

test: clean build_tests run_tests

install: install_inja

install_inja:
	cd ./lib/inja && cmake . -G "Unix Makefiles" && make MAKE=make CMAKE_COMMAND=cmake

build:
	$(COMPILER_CPP) $(CPP_STD) $(OPTIMIZATION_LEVEL) $(OPT) ./src/main.cpp -o ./bin/main.exe $(INCLUDE_DIRS) $(WARNING_FLAGS) $(LIB_FLAGS) $(INJACPP)

build_tests:
	$(COMPILER_CPP) $(CPP_STD) $(OPTIMIZATION_LEVEL) $(OPT) ./tests/main.cpp -o ./bin/test.exe $(INCLUDE_DIRS) $(WARNING_FLAGS) $(LIB_FLAGS) $(INJACPP)

debug_build:
	$(COMPILER_CPP) $(CPP_STD) $(OPTIMIZATION_LEVEL) $(OPT) ./src/main.cpp -o ./bin/main.exe $(INCLUDE_DIRS) $(WARNING_FLAGS) $(LIB_FLAGS) $(DEBUG_FLAGS) $(INJACPP)

debug_build_tests:
	$(COMPILER_CPP) $(CPP_STD) $(OPTIMIZATION_LEVEL) $(OPT) ./tests/main.cpp -o ./bin/test.exe $(INCLUDE_DIRS) $(WARNING_FLAGS) $(LIB_FLAGS) $(DEBUG_FLAGS) $(INJACPP)

run:
	./bin/main.exe

run_tests:
	mkdir -p ./bin/log
	./bin/test.exe
	cat ./bin/log/performance_summary.txt

clean:
	rm -rf ./bin
	mkdir -p ./bin

benchmark:
	$(COMPILER_CPP) $(CPP_STD) $(OPT) ./benchmark/contestants/server.cpp -o ./benchmark/contestants/server.exe $(INCLUDE_DIRS) $(LIB_FLAGS) $(INJACPP) -fconcepts; \
	npm install express; \
	python -m pip install flask; \
	./benchmark/contestants/server.exe & \
	python benchmark/contestants/server.py & \
	node benchmark/contestants/server.js & \
	sleep 10s; \
	cd benchmark; \
	mkdir tmp; \
	bash benchmark.sh

save-benchmark:
	cd benchmark; \
	cp tmp/*.result results/ 

local-benchmark:
	mkdir -p ./benchmark/performance/$(DIR_PREFIX)\
	for i in "" 0 1 2 3 fast g s; do \
	 	tmieout 1h make -B clean build_tests run_tests COMPILER_CPP=$(COMPILER_CPP) OPTIMIZATION_LEVEL=-O$$i ; \
		cp ./bin/log/performance.txt ./benchmark/performance/$(DIR_PREFIX)performance-O$$i.txt ; \
		cp ./bin/log/performance_summary.txt ./benchmark/$(DIR_PREFIX)performance/performance_summary-O$$i.txt ; \
	done