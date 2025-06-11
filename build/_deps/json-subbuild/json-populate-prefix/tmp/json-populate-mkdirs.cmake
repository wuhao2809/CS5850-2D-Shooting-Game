# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/haowu/Desktop/code/projects/2D Shooting Game/CS5850-2D-Shooting-Game/build/_deps/json-src"
  "/Users/haowu/Desktop/code/projects/2D Shooting Game/CS5850-2D-Shooting-Game/build/_deps/json-build"
  "/Users/haowu/Desktop/code/projects/2D Shooting Game/CS5850-2D-Shooting-Game/build/_deps/json-subbuild/json-populate-prefix"
  "/Users/haowu/Desktop/code/projects/2D Shooting Game/CS5850-2D-Shooting-Game/build/_deps/json-subbuild/json-populate-prefix/tmp"
  "/Users/haowu/Desktop/code/projects/2D Shooting Game/CS5850-2D-Shooting-Game/build/_deps/json-subbuild/json-populate-prefix/src/json-populate-stamp"
  "/Users/haowu/Desktop/code/projects/2D Shooting Game/CS5850-2D-Shooting-Game/build/_deps/json-subbuild/json-populate-prefix/src"
  "/Users/haowu/Desktop/code/projects/2D Shooting Game/CS5850-2D-Shooting-Game/build/_deps/json-subbuild/json-populate-prefix/src/json-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/haowu/Desktop/code/projects/2D Shooting Game/CS5850-2D-Shooting-Game/build/_deps/json-subbuild/json-populate-prefix/src/json-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/haowu/Desktop/code/projects/2D Shooting Game/CS5850-2D-Shooting-Game/build/_deps/json-subbuild/json-populate-prefix/src/json-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
