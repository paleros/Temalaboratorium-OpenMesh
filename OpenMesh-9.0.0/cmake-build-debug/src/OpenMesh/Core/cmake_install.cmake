# Install script for directory: /home/peros/Documents/CLionProjects/C++/Temalaboratorium/OpenMeshTest/OpenMesh-9.0.0/src/OpenMesh/Core

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/peros/Documents/CLionProjects/C++/Temalaboratorium/OpenMeshTest/OpenMesh-9.0.0/cmake-build-debug/Build/lib/libOpenMeshCored.so.9.0")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libOpenMeshCored.so.9.0" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libOpenMeshCored.so.9.0")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libOpenMeshCored.so.9.0")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/peros/Documents/CLionProjects/C++/Temalaboratorium/OpenMeshTest/OpenMesh-9.0.0/cmake-build-debug/Build/lib/libOpenMeshCored.so")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/peros/Documents/CLionProjects/C++/Temalaboratorium/OpenMeshTest/OpenMesh-9.0.0/cmake-build-debug/src/OpenMesh/Core/libOpenMeshCored.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/OpenMesh/Core" TYPE DIRECTORY FILES "/home/peros/Documents/CLionProjects/C++/Temalaboratorium/OpenMeshTest/OpenMesh-9.0.0/src/OpenMesh/Core/." FILES_MATCHING REGEX "/[^/]*\\.hh$" REGEX "/CVS$" EXCLUDE REGEX "/\\.svn$" EXCLUDE REGEX "/tmp$" EXCLUDE REGEX "/Templates$" EXCLUDE REGEX "/Debian[^/]*$" EXCLUDE)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/OpenMesh/Core/System" TYPE FILE FILES "/home/peros/Documents/CLionProjects/C++/Temalaboratorium/OpenMeshTest/OpenMesh-9.0.0/src/OpenMesh/Core/System/config.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/OpenMesh/Core/IO" TYPE DIRECTORY FILES "/home/peros/Documents/CLionProjects/C++/Temalaboratorium/OpenMeshTest/OpenMesh-9.0.0/src/OpenMesh/Core/IO/" FILES_MATCHING REGEX "/[^/]*\\.inl$" REGEX "/CVS$" EXCLUDE REGEX "/\\.svn$" EXCLUDE REGEX "/reader$" EXCLUDE REGEX "/writer$" EXCLUDE REGEX "/importer$" EXCLUDE REGEX "/exporter$" EXCLUDE REGEX "/tmp$" EXCLUDE REGEX "/Debian[^/]*$" EXCLUDE)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/peros/Documents/CLionProjects/C++/Temalaboratorium/OpenMeshTest/OpenMesh-9.0.0/cmake-build-debug/Build/lib/libOpenMeshCored.so.9.0")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libOpenMeshCored.so.9.0" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libOpenMeshCored.so.9.0")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libOpenMeshCored.so.9.0")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/peros/Documents/CLionProjects/C++/Temalaboratorium/OpenMeshTest/OpenMesh-9.0.0/cmake-build-debug/Build/lib/libOpenMeshCored.so")
endif()

