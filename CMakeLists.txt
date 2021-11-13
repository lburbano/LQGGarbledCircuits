cmake_minimum_required (VERSION 2.8.11)
project (GC)
set(NAME "GC")

find_path(CMAKE_FOLDER NAMES cmake/emp-tool-config.cmake)
include(${CMAKE_FOLDER}/cmake/emp-base.cmake)

find_package(emp-ot REQUIRED)
include_directories(${EMP-OT_INCLUDE_DIRS})
include_directories(include)

file(COPY ${CMAKE_SOURCE_DIR}/Data DESTINATION ./)

# Installation
install(FILES cmake/emp-sh2pc-config.cmake DESTINATION cmake/)
install(DIRECTORY LQGGarbledCircuits DESTINATION include/)

ENABLE_TESTING()
ADD_SUBDIRECTORY(test)