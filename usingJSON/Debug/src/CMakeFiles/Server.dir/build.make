# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

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
CMAKE_SOURCE_DIR = /home/andrewh/Source/Mine/powerManagement/usingJSON

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/andrewh/Source/Mine/powerManagement/usingJSON/Debug

# Include any dependencies generated for this target.
include src/CMakeFiles/Server.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/Server.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/Server.dir/flags.make

src/CMakeFiles/Server.dir/Server.cpp.o: src/CMakeFiles/Server.dir/flags.make
src/CMakeFiles/Server.dir/Server.cpp.o: ../src/Server.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/andrewh/Source/Mine/powerManagement/usingJSON/Debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/Server.dir/Server.cpp.o"
	cd /home/andrewh/Source/Mine/powerManagement/usingJSON/Debug/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Server.dir/Server.cpp.o -c /home/andrewh/Source/Mine/powerManagement/usingJSON/src/Server.cpp

src/CMakeFiles/Server.dir/Server.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Server.dir/Server.cpp.i"
	cd /home/andrewh/Source/Mine/powerManagement/usingJSON/Debug/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/andrewh/Source/Mine/powerManagement/usingJSON/src/Server.cpp > CMakeFiles/Server.dir/Server.cpp.i

src/CMakeFiles/Server.dir/Server.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Server.dir/Server.cpp.s"
	cd /home/andrewh/Source/Mine/powerManagement/usingJSON/Debug/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/andrewh/Source/Mine/powerManagement/usingJSON/src/Server.cpp -o CMakeFiles/Server.dir/Server.cpp.s

# Object files for target Server
Server_OBJECTS = \
"CMakeFiles/Server.dir/Server.cpp.o"

# External object files for target Server
Server_EXTERNAL_OBJECTS =

src/Server: src/CMakeFiles/Server.dir/Server.cpp.o
src/Server: src/CMakeFiles/Server.dir/build.make
src/Server: src/CMakeFiles/Server.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/andrewh/Source/Mine/powerManagement/usingJSON/Debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Server"
	cd /home/andrewh/Source/Mine/powerManagement/usingJSON/Debug/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Server.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/Server.dir/build: src/Server

.PHONY : src/CMakeFiles/Server.dir/build

src/CMakeFiles/Server.dir/clean:
	cd /home/andrewh/Source/Mine/powerManagement/usingJSON/Debug/src && $(CMAKE_COMMAND) -P CMakeFiles/Server.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/Server.dir/clean

src/CMakeFiles/Server.dir/depend:
	cd /home/andrewh/Source/Mine/powerManagement/usingJSON/Debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/andrewh/Source/Mine/powerManagement/usingJSON /home/andrewh/Source/Mine/powerManagement/usingJSON/src /home/andrewh/Source/Mine/powerManagement/usingJSON/Debug /home/andrewh/Source/Mine/powerManagement/usingJSON/Debug/src /home/andrewh/Source/Mine/powerManagement/usingJSON/Debug/src/CMakeFiles/Server.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/Server.dir/depend

