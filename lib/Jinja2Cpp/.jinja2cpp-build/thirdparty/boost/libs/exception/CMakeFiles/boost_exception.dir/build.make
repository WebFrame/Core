# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/alex/Core/lib/Jinja2Cpp

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/alex/Core/lib/Jinja2Cpp/.jinja2cpp-build

# Include any dependencies generated for this target.
include thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/depend.make

# Include the progress variables for this target.
include thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/progress.make

# Include the compile flags for this target's objects.
include thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/flags.make

thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/src/clone_current_exception_non_intrusive.cpp.o: thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/flags.make
thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/src/clone_current_exception_non_intrusive.cpp.o: ../thirdparty/boost/libs/exception/src/clone_current_exception_non_intrusive.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alex/Core/lib/Jinja2Cpp/.jinja2cpp-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/src/clone_current_exception_non_intrusive.cpp.o"
	cd /home/alex/Core/lib/Jinja2Cpp/.jinja2cpp-build/thirdparty/boost/libs/exception && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/boost_exception.dir/src/clone_current_exception_non_intrusive.cpp.o -c /home/alex/Core/lib/Jinja2Cpp/thirdparty/boost/libs/exception/src/clone_current_exception_non_intrusive.cpp

thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/src/clone_current_exception_non_intrusive.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/boost_exception.dir/src/clone_current_exception_non_intrusive.cpp.i"
	cd /home/alex/Core/lib/Jinja2Cpp/.jinja2cpp-build/thirdparty/boost/libs/exception && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/alex/Core/lib/Jinja2Cpp/thirdparty/boost/libs/exception/src/clone_current_exception_non_intrusive.cpp > CMakeFiles/boost_exception.dir/src/clone_current_exception_non_intrusive.cpp.i

thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/src/clone_current_exception_non_intrusive.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/boost_exception.dir/src/clone_current_exception_non_intrusive.cpp.s"
	cd /home/alex/Core/lib/Jinja2Cpp/.jinja2cpp-build/thirdparty/boost/libs/exception && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/alex/Core/lib/Jinja2Cpp/thirdparty/boost/libs/exception/src/clone_current_exception_non_intrusive.cpp -o CMakeFiles/boost_exception.dir/src/clone_current_exception_non_intrusive.cpp.s

thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/src/clone_current_exception_non_intrusive.cpp.o.requires:

.PHONY : thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/src/clone_current_exception_non_intrusive.cpp.o.requires

thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/src/clone_current_exception_non_intrusive.cpp.o.provides: thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/src/clone_current_exception_non_intrusive.cpp.o.requires
	$(MAKE) -f thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/build.make thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/src/clone_current_exception_non_intrusive.cpp.o.provides.build
.PHONY : thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/src/clone_current_exception_non_intrusive.cpp.o.provides

thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/src/clone_current_exception_non_intrusive.cpp.o.provides.build: thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/src/clone_current_exception_non_intrusive.cpp.o


# Object files for target boost_exception
boost_exception_OBJECTS = \
"CMakeFiles/boost_exception.dir/src/clone_current_exception_non_intrusive.cpp.o"

# External object files for target boost_exception
boost_exception_EXTERNAL_OBJECTS =

thirdparty/boost/libs/exception/libboost_exception.a: thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/src/clone_current_exception_non_intrusive.cpp.o
thirdparty/boost/libs/exception/libboost_exception.a: thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/build.make
thirdparty/boost/libs/exception/libboost_exception.a: thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/alex/Core/lib/Jinja2Cpp/.jinja2cpp-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libboost_exception.a"
	cd /home/alex/Core/lib/Jinja2Cpp/.jinja2cpp-build/thirdparty/boost/libs/exception && $(CMAKE_COMMAND) -P CMakeFiles/boost_exception.dir/cmake_clean_target.cmake
	cd /home/alex/Core/lib/Jinja2Cpp/.jinja2cpp-build/thirdparty/boost/libs/exception && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/boost_exception.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/build: thirdparty/boost/libs/exception/libboost_exception.a

.PHONY : thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/build

thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/requires: thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/src/clone_current_exception_non_intrusive.cpp.o.requires

.PHONY : thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/requires

thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/clean:
	cd /home/alex/Core/lib/Jinja2Cpp/.jinja2cpp-build/thirdparty/boost/libs/exception && $(CMAKE_COMMAND) -P CMakeFiles/boost_exception.dir/cmake_clean.cmake
.PHONY : thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/clean

thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/depend:
	cd /home/alex/Core/lib/Jinja2Cpp/.jinja2cpp-build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/alex/Core/lib/Jinja2Cpp /home/alex/Core/lib/Jinja2Cpp/thirdparty/boost/libs/exception /home/alex/Core/lib/Jinja2Cpp/.jinja2cpp-build /home/alex/Core/lib/Jinja2Cpp/.jinja2cpp-build/thirdparty/boost/libs/exception /home/alex/Core/lib/Jinja2Cpp/.jinja2cpp-build/thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : thirdparty/boost/libs/exception/CMakeFiles/boost_exception.dir/depend

