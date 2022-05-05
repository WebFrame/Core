COMPILER_CPP=g++
CPP_STD=-std=c++2a
OPTIMIZATION_LEVEL=-O2
OPT=-fconstexpr-depth=700
INCLUDE_DIRS=-I./tests -I./lib/
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

all: build_all run

test: clean build_tests run_tests

install: install_inja

install_inja:
	cd ./lib/inja && cmake . -G "Unix Makefiles" && make MAKE=make CMAKE_COMMAND=cmake

build_all: build build_tests benchmark_build

build:
	$(COMPILER_CPP) $(CPP_STD) $(OPTIMIZATION_LEVEL) $(OPT) ./example/main.cpp -o ./bin/main.exe $(INCLUDE_DIRS) $(WARNING_FLAGS) $(LIB_FLAGS) $(INJACPP)

build_tests:
	$(COMPILER_CPP) $(CPP_STD) $(OPTIMIZATION_LEVEL) $(OPT) ./tests/main.cpp -o ./bin/test.exe $(INCLUDE_DIRS) $(WARNING_FLAGS) $(LIB_FLAGS) $(INJACPP)

debug_build_all: debug_build debug_build_tests debug_benchmark_build

debug_build:
	$(COMPILER_CPP) $(CPP_STD) $(OPTIMIZATION_LEVEL) $(OPT) ./example/main.cpp -o ./bin/main.exe $(INCLUDE_DIRS) $(WARNING_FLAGS) $(LIB_FLAGS) $(DEBUG_FLAGS) $(INJACPP)

debug_build_tests:
	$(COMPILER_CPP) $(CPP_STD) $(OPTIMIZATION_LEVEL) $(OPT) ./tests/main.cpp -o ./bin/test.exe $(INCLUDE_DIRS) $(WARNING_FLAGS) $(LIB_FLAGS) $(DEBUG_FLAGS) $(INJACPP)

benchmark_build:
	$(COMPILER_CPP) $(CPP_STD) $(OPT) -O     ./benchmark/contestants/server_atomic.cpp -o ./benchmark/contestants/server_atomic-O.exe      $(INCLUDE_DIRS) $(LIB_FLAGS) $(INJACPP)
	$(COMPILER_CPP) $(CPP_STD) $(OPT) -O1    ./benchmark/contestants/server_atomic.cpp -o ./benchmark/contestants/server_atomic-O1.exe     $(INCLUDE_DIRS) $(LIB_FLAGS) $(INJACPP)
	$(COMPILER_CPP) $(CPP_STD) $(OPT) -O2    ./benchmark/contestants/server_atomic.cpp -o ./benchmark/contestants/server_atomic-O2.exe     $(INCLUDE_DIRS) $(LIB_FLAGS) $(INJACPP)
	$(COMPILER_CPP) $(CPP_STD) $(OPT) -O3    ./benchmark/contestants/server_atomic.cpp -o ./benchmark/contestants/server_atomic-O3.exe     $(INCLUDE_DIRS) $(LIB_FLAGS) $(INJACPP)
	$(COMPILER_CPP) $(CPP_STD) $(OPT) -Ofast ./benchmark/contestants/server_atomic.cpp -o ./benchmark/contestants/server_atomic-Ofast.exe  $(INCLUDE_DIRS) $(LIB_FLAGS) $(INJACPP)
	$(COMPILER_CPP) $(CPP_STD) $(OPT) -Og    ./benchmark/contestants/server_atomic.cpp -o ./benchmark/contestants/server_atomic-Og.exe     $(INCLUDE_DIRS) $(LIB_FLAGS) $(INJACPP)
	$(COMPILER_CPP) $(CPP_STD) $(OPT) -Os    ./benchmark/contestants/server_atomic.cpp -o ./benchmark/contestants/server_atomic-Os.exe     $(INCLUDE_DIRS) $(LIB_FLAGS) $(INJACPP)

	$(COMPILER_CPP) $(CPP_STD) $(OPT) -O     ./benchmark/contestants/server.cpp -o ./benchmark/contestants/server-O.exe      $(INCLUDE_DIRS) $(LIB_FLAGS) $(INJACPP)
	$(COMPILER_CPP) $(CPP_STD) $(OPT) -O1    ./benchmark/contestants/server.cpp -o ./benchmark/contestants/server-O1.exe     $(INCLUDE_DIRS) $(LIB_FLAGS) $(INJACPP)
	$(COMPILER_CPP) $(CPP_STD) $(OPT) -O2    ./benchmark/contestants/server.cpp -o ./benchmark/contestants/server-O2.exe     $(INCLUDE_DIRS) $(LIB_FLAGS) $(INJACPP)
	$(COMPILER_CPP) $(CPP_STD) $(OPT) -O3    ./benchmark/contestants/server.cpp -o ./benchmark/contestants/server-O3.exe     $(INCLUDE_DIRS) $(LIB_FLAGS) $(INJACPP)
	$(COMPILER_CPP) $(CPP_STD) $(OPT) -Ofast ./benchmark/contestants/server.cpp -o ./benchmark/contestants/server-Ofast.exe  $(INCLUDE_DIRS) $(LIB_FLAGS) $(INJACPP)
	$(COMPILER_CPP) $(CPP_STD) $(OPT) -Og    ./benchmark/contestants/server.cpp -o ./benchmark/contestants/server-Og.exe     $(INCLUDE_DIRS) $(LIB_FLAGS) $(INJACPP)
	$(COMPILER_CPP) $(CPP_STD) $(OPT) -Os    ./benchmark/contestants/server.cpp -o ./benchmark/contestants/server-Os.exe     $(INCLUDE_DIRS) $(LIB_FLAGS) $(INJACPP)

debug_benchmark_build:
	$(COMPILER_CPP) $(CPP_STD) $(OPT) ./benchmark/contestants/server.cpp -o ./benchmark/contestants/server.exe $(INCLUDE_DIRS) $(LIB_FLAGS) $(DEBUG_FLAGS) $(INJACPP)

clean:
	rm -rf ./bin
	mkdir -p ./bin/log

run:
	./bin/main.exe

run_tests:
	./bin/test.exe

benchmark: benchmark_build
	npm install express
	python -m pip install flask

	./benchmark/contestants/server-O.exe 8888 &
	./benchmark/contestants/server-O1.exe 8889 &
	./benchmark/contestants/server-O2.exe 8890 &
	./benchmark/contestants/server-O3.exe 8891 &
	./benchmark/contestants/server-Ofast.exe 8892 &
	./benchmark/contestants/server-Og.exe 8893 &
	./benchmark/contestants/server-Os.exe 8894 &
	
	./benchmark/contestants/server_atomic-O.exe 8895 &
	./benchmark/contestants/server_atomic-O1.exe 8896 &
	./benchmark/contestants/server_atomic-O2.exe 8897 &
	./benchmark/contestants/server_atomic-O3.exe 8898 &
	./benchmark/contestants/server_atomic-Ofast.exe 8899 &
	./benchmark/contestants/server_atomic-Og.exe 8900 &
	./benchmark/contestants/server_atomic-Os.exe 8901 &

	python benchmark/contestants/server.py &
	
	node benchmark/contestants/server.js &
	
	sleep 10s;
	cd benchmark;
	mkdir tmp;
	bash benchmark.sh;

save-benchmark:
	cd benchmark; \
	cp tmp/*.result results/ 

local-benchmark:
	mkdir -p ./benchmark/performance/$(DIR_PREFIX); \
	make -B clean build_tests run_tests COMPILER_CPP=$(COMPILER_CPP) OPTIMIZATION_LEVEL=-O ; \
 
	make -B clean build_tests run_tests COMPILER_CPP=$(COMPILER_CPP) OPTIMIZATION_LEVEL=-O1 ; \
 
	make -B clean build_tests run_tests COMPILER_CPP=$(COMPILER_CPP) OPTIMIZATION_LEVEL=-O2 ; \

	make -B clean build_tests run_tests COMPILER_CPP=$(COMPILER_CPP) OPTIMIZATION_LEVEL=-O3 ; \

	make -B clean build_tests run_tests COMPILER_CPP=$(COMPILER_CPP) OPTIMIZATION_LEVEL=-Ofast ; \

	make -B clean build_tests run_tests COMPILER_CPP=$(COMPILER_CPP) OPTIMIZATION_LEVEL=-Og ; \

	make -B clean build_tests run_tests COMPILER_CPP=$(COMPILER_CPP) OPTIMIZATION_LEVEL=-Os ; \
