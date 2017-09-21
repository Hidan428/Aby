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

	set(SOURCE_FILES src/main.cpp
	                 src/window.cpp
	                 src/error.cpp)

	add_executable(aby ${SOURCE_FILES})

	find_package(PkgConfig)
	pkg_search_module(GLEW REQUIRED glew)
	pkg_search_module(GLFW REQUIRED glfw3)

	if(UNIX AND NOT APPLE)
		pkg_search_module(GL REQUIRED glu)

		target_include_directories(aby PUBLIC ${PROJECT_SOURCE_DIR}/include)
	endif()

	if(APPLE)		
		target_include_directories(aby PUBLIC ${PROJECT_SOURCE_DIR}/include System/Library/Frameworks)
		find_library(COCOA_LIBRARY Cocoa)
		find_library(OpenGL_LIBRARY OpenGL)
	
		set(OSX_LIBS ${COCOA_LIBRARY} ${OpenGL_LIBRARY})
	endif()

	target_link_libraries(aby ${OSX_LIBS} ${GL_LIBRARIES} ${GLEW_STATIC_LIBRARIES} ${GLFW_STATIC_LIBRARIES})
	target_include_directories(aby PUBLIC ${GL_INCLUDE_DIRS})
	target_compile_options(aby PUBLIC ${GLEW_CFLAGS_OTHER} ${GLFW3_CFLAGS_OTHER} ${GL_CFLAGS_OTHER})
endif()

	
