cmake_minimum_required(VERSION 3.17)

project(
  fge
  VERSION 0.0.1
  DESCRIPTION "chip8_simulator - Simulator for chip8"
  LANGUAGES CXX C
  )

# Let's ensure -std=c++xx instead of -std=g++xx
set(CMAKE_CXX_EXTENSIONS OFF)

# Colored compiler output
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  add_compile_options (-fdiagnostics-color=always)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  add_compile_options (-fcolor-diagnostics)
endif ()

add_subdirectory(external)
add_subdirectory(src)
