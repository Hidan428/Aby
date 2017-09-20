cmake_minimum_required(VERSION 3.2)

option(BUILD_GAME "Build the game" ON)
option(BUILD_DOCS "Build the documentation" OFF)
option(ENABLE_TESTS "Enable the tests" OFF)

project(aby)

if(BUILD_GAME)
	set(CMAKE_CXX_STANDARD 14)

	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
	set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
	set(CMAKE_CXX_FLAGS "-Wall -pedantic")

	set(SOURCE_FILES src/main.cpp)

	include_directories(${PROJECT_SOURCE_DIR}/include)

	add_executable(aby ${SOURCE_FILES})
endif()

	
