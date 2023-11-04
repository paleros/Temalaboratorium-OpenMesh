cmake_minimum_required(VERSION 3.25)
project(OpenMeshTest)

set(CMAKE_CXX_STANDARD 17)
#add_library( OpenMeshCore STATIC )

# Create imported target OpenMeshCore
add_library(OpenMeshCore SHARED IMPORTED)

set_target_properties(OpenMeshCore PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${_IMPORT_PREFIX}/include"
)

# Create imported target OpenMeshTools
add_library(OpenMeshTools SHARED IMPORTED)

add_executable(OpenMeshTest main.cpp)
