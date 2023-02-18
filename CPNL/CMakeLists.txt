cmake_minimum_required(VERSION 3.8)

set(PROJECT_NAME CPNL32r)

project(${PROJECT_NAME} VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 14)
if (UNIX AND NOT APPLE)
set(CMAKE_CXX_FLAGS -pthread)
endif (UNIX AND NOT APPLE)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

file(GLOB SOURCE_FILES "Resource Files/*.cpp")

add_executable(${PROJECT_NAME} "Source Files/Main.cpp" ${SOURCE_FILES})
#add_library(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC "Header Files")