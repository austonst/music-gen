# Copyright (c) 2014 Auston Sterling
# See LICENSE for copying permission.

cmake_minimum_required(VERSION 2.8.8)

project(music)

# Enable C++11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# Link the midi library
find_library(MIDI_LIB midi
  HINTS ${MIDI_ROOT}/lib)
find_path(MIDI_INCLUDE midi/midi.hpp
  HINTS ${MIDI_ROOT}/include)
if (MIDI_LIB AND MIDI_INCLUDE)
  message(STATUS "libmidi found: ${MIDI_INCLUDE} ${MIDI_LIB}")
else()
  message(FATAL_ERROR "libmidi NOT found!")
endif()

# All libraries loaded; include them
include_directories(${MIDI_INCLUDE})

# Create the various test programs
set(TESTMOTIF_SRCS
  ./motif.cpp
  ./testmotif.cpp)
add_executable(testmotif ${TESTMOTIF_SRCS})
target_link_libraries(testmotif ${MIDI_LIB})

set(TESTTHEME_SRCS
  ./motif.cpp
  ./theme.cpp
  ./testtheme.cpp)
add_executable(testtheme ${TESTTHEME_SRCS})
target_link_libraries(testtheme ${MIDI_LIB})

set(TESTPIECE_SRCS
  ./motif.cpp
  ./theme.cpp
  ./piece.cpp
  ./testpiece.cpp)
add_executable(testpiece ${TESTPIECE_SRCS})
target_link_libraries(testpiece ${MIDI_LIB})