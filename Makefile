COMPILER_CPP=g++
CPP_STD=-std=c++2a
OPTIMIZATION_LEVEL=-O2
OPT=-fconstexpr-depth=700
INCLUDE_DIRS=-I./tests -I./src
LIB_FLAGS=-pthread -lpthread
DEBUG_FLAGS=-fsanitize=undefined -fsanitize=address
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

build_all: build build_tests benchmark_build

build:
	$(COMPILER_CPP) $(CPP_STD) $(OPTIMIZATION_LEVEL) $(OPT) ./src/main.cpp -o ./bin/main.exe $(INCLUDE_DIRS) $(WARNING_FLAGS) $(LIB_FLAGS) $(INJACPP)

build_tests:
	$(COMPILER_CPP) $(CPP_STD) $(OPTIMIZATION_LEVEL) $(OPT) ./tests/main.cpp -o ./bin/test.exe $(INCLUDE_DIRS) $(WARNING_FLAGS) $(LIB_FLAGS) $(INJACPP)

debug_build_all: debug_build debug_build_tests debug_benchmark_build

debug_build:
	$(COMPILER_CPP) $(CPP_STD) $(OPTIMIZATION_LEVEL) $(OPT) ./src/main.cpp -o ./bin/main.exe $(INCLUDE_DIRS) $(WARNING_FLAGS) $(LIB_FLAGS) $(DEBUG_FLAGS) $(INJACPP)

debug_build_tests:
	$(COMPILER_CPP) $(CPP_STD) $(OPTIMIZATION_LEVEL) $(OPT) ./tests/main.cpp -o ./bin/test.exe $(INCLUDE_DIRS) $(WARNING_FLAGS) $(LIB_FLAGS) $(DEBUG_FLAGS) $(INJACPP)

benchmark_build:
	$(COMPILER_CPP) $(CPP_STD) $(OPT) ./benchmark/contestants/server.cpp -o ./benchmark/contestants/server.exe $(INCLUDE_DIRS) $(LIB_FLAGS) $(INJACPP) -fconcepts

debug_benchmark_build:
	$(COMPILER_CPP) $(CPP_STD) $(OPT) ./benchmark/contestants/server.cpp -o ./benchmark/contestants/server.exe $(INCLUDE_DIRS) $(LIB_FLAGS) $(DEBUG_FLAGS) $(INJACPP) -fconcepts

clean:
	rm -rf ./bin
	mkdir -p ./bin/log

run:
	./bin/main.exe

run_tests:
	./bin/test.exe
	cat ./bin/log/performance_summary.txt

benchmark: benchmark_build
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
	mkdir -p ./benchmark/performance/$(DIR_PREFIX); \
	make -B clean build_tests run_tests COMPILER_CPP=$(COMPILER_CPP) OPTIMIZATION_LEVEL=-O ; \
	cp ./bin/log/performance.txt ./benchmark/performance/$(DIR_PREFIX)performance-O.txt ; \
	cp ./bin/log/performance_summary.txt ./benchmark/performance/$(DIR_PREFIX)performance_summary-O.txt ; \
 
	make -B clean build_tests run_tests COMPILER_CPP=$(COMPILER_CPP) OPTIMIZATION_LEVEL=-O1 ; \
	cp ./bin/log/performance.txt ./benchmark/performance/$(DIR_PREFIX)performance-O1.txt ; \
	cp ./bin/log/performance_summary.txt ./benchmark/performance/$(DIR_PREFIX)performance_summary-O1.txt ; \
 
	make -B clean build_tests run_tests COMPILER_CPP=$(COMPILER_CPP) OPTIMIZATION_LEVEL=-O2 ; \
	cp ./bin/log/performance.txt ./benchmark/performance/$(DIR_PREFIX)performance-O2.txt ; \
	cp ./bin/log/performance_summary.txt ./benchmark/performance/$(DIR_PREFIX)performance_summary-O2.txt ; \
 
	make -B clean build_tests run_tests COMPILER_CPP=$(COMPILER_CPP) OPTIMIZATION_LEVEL=-O3 ; \
	cp ./bin/log/performance.txt ./benchmark/performance/$(DIR_PREFIX)performance-O3.txt ; \
	cp ./bin/log/performance_summary.txt ./benchmark/performance/$(DIR_PREFIX)performance_summary-O3.txt ; \

	make -B clean build_tests run_tests COMPILER_CPP=$(COMPILER_CPP) OPTIMIZATION_LEVEL=-Ofast ; \
	cp ./bin/log/performance.txt ./benchmark/performance/$(DIR_PREFIX)performance-Ofast.txt ; \
	cp ./bin/log/performance_summary.txt ./benchmark/performance/$(DIR_PREFIX)performance_summary-Ofast.txt ; \

	make -B clean build_tests run_tests COMPILER_CPP=$(COMPILER_CPP) OPTIMIZATION_LEVEL=-Og ; \
	cp ./bin/log/performance.txt ./benchmark/performance/$(DIR_PREFIX)performance-Og.txt ; \
	cp ./bin/log/performance_summary.txt ./benchmark/performance/$(DIR_PREFIX)performance_summary-Og.txt ; \

	make -B clean build_tests run_tests COMPILER_CPP=$(COMPILER_CPP) OPTIMIZATION_LEVEL=-Os ; \
	cp ./bin/log/performance.txt ./benchmark/performance/$(DIR_PREFIX)performance-Os.txt ; \
	cp ./bin/log/performance_summary.txt ./benchmark/performance/$(DIR_PREFIX)performance_summary-Os.txt ; \