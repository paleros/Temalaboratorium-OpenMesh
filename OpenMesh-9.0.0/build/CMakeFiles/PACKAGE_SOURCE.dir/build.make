# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.27

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/peros/Documents/CLion/C++/Temalabor/OpenMeshTest/OpenMesh-9.0.0

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/peros/Documents/CLion/C++/Temalabor/OpenMeshTest/OpenMesh-9.0.0/build

# Utility rule file for PACKAGE_SOURCE.

# Include any custom commands dependencies for this target.
include CMakeFiles/PACKAGE_SOURCE.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/PACKAGE_SOURCE.dir/progress.make

CMakeFiles/PACKAGE_SOURCE:
	/usr/bin/cpack --config /home/peros/Documents/CLion/C++/Temalabor/OpenMeshTest/OpenMesh-9.0.0/build/CPackSourceConfig.cmake

PACKAGE_SOURCE: CMakeFiles/PACKAGE_SOURCE
PACKAGE_SOURCE: CMakeFiles/PACKAGE_SOURCE.dir/build.make
.PHONY : PACKAGE_SOURCE

# Rule to build all files generated by this target.
CMakeFiles/PACKAGE_SOURCE.dir/build: PACKAGE_SOURCE
.PHONY : CMakeFiles/PACKAGE_SOURCE.dir/build

CMakeFiles/PACKAGE_SOURCE.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/PACKAGE_SOURCE.dir/cmake_clean.cmake
.PHONY : CMakeFiles/PACKAGE_SOURCE.dir/clean

CMakeFiles/PACKAGE_SOURCE.dir/depend:
	cd /home/peros/Documents/CLion/C++/Temalabor/OpenMeshTest/OpenMesh-9.0.0/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/peros/Documents/CLion/C++/Temalabor/OpenMeshTest/OpenMesh-9.0.0 /home/peros/Documents/CLion/C++/Temalabor/OpenMeshTest/OpenMesh-9.0.0 /home/peros/Documents/CLion/C++/Temalabor/OpenMeshTest/OpenMesh-9.0.0/build /home/peros/Documents/CLion/C++/Temalabor/OpenMeshTest/OpenMesh-9.0.0/build /home/peros/Documents/CLion/C++/Temalabor/OpenMeshTest/OpenMesh-9.0.0/build/CMakeFiles/PACKAGE_SOURCE.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/PACKAGE_SOURCE.dir/depend

