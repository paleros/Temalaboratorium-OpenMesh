# Install script for directory: /mnt/Storage/Storage/projekt/CLionProjects/C++/Temalaboratorium/Temalaboratorium-OpenMesh/OpenMesh-9.0.0

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/libdata/pkgconfig" TYPE FILE FILES "/mnt/Storage/Storage/projekt/CLionProjects/C++/Temalaboratorium/Temalaboratorium-OpenMesh/OpenMesh-9.0.0/cmake-build-debug/openmesh.pc")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/OpenMesh/cmake/OpenMeshConfig.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/OpenMesh/cmake/OpenMeshConfig.cmake"
         "/mnt/Storage/Storage/projekt/CLionProjects/C++/Temalaboratorium/Temalaboratorium-OpenMesh/OpenMesh-9.0.0/cmake-build-debug/CMakeFiles/Export/14b769bc31fd504a15bc5c93136aeb1f/OpenMeshConfig.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/OpenMesh/cmake/OpenMeshConfig-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/OpenMesh/cmake/OpenMeshConfig.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/OpenMesh/cmake" TYPE FILE FILES "/mnt/Storage/Storage/projekt/CLionProjects/C++/Temalaboratorium/Temalaboratorium-OpenMesh/OpenMesh-9.0.0/cmake-build-debug/CMakeFiles/Export/14b769bc31fd504a15bc5c93136aeb1f/OpenMeshConfig.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/OpenMesh/cmake" TYPE FILE FILES "/mnt/Storage/Storage/projekt/CLionProjects/C++/Temalaboratorium/Temalaboratorium-OpenMesh/OpenMesh-9.0.0/cmake-build-debug/CMakeFiles/Export/14b769bc31fd504a15bc5c93136aeb1f/OpenMeshConfig-debug.cmake")
  endif()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/mnt/Storage/Storage/projekt/CLionProjects/C++/Temalaboratorium/Temalaboratorium-OpenMesh/OpenMesh-9.0.0/cmake-build-debug/src/OpenMesh/Core/cmake_install.cmake")
  include("/mnt/Storage/Storage/projekt/CLionProjects/C++/Temalaboratorium/Temalaboratorium-OpenMesh/OpenMesh-9.0.0/cmake-build-debug/src/OpenMesh/Tools/cmake_install.cmake")
  include("/mnt/Storage/Storage/projekt/CLionProjects/C++/Temalaboratorium/Temalaboratorium-OpenMesh/OpenMesh-9.0.0/cmake-build-debug/src/OpenMesh/Apps/cmake_install.cmake")
  include("/mnt/Storage/Storage/projekt/CLionProjects/C++/Temalaboratorium/Temalaboratorium-OpenMesh/OpenMesh-9.0.0/cmake-build-debug/src/Unittests/cmake_install.cmake")
  include("/mnt/Storage/Storage/projekt/CLionProjects/C++/Temalaboratorium/Temalaboratorium-OpenMesh/OpenMesh-9.0.0/cmake-build-debug/Doc/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/mnt/Storage/Storage/projekt/CLionProjects/C++/Temalaboratorium/Temalaboratorium-OpenMesh/OpenMesh-9.0.0/cmake-build-debug/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
