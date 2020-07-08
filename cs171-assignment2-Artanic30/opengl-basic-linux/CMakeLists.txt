cmake_minimum_required(VERSION 3.15)
project(opengl-basic)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 14)

add_executable(opengl-basic src/main.cpp inc/glad.c inc/shader_m.h inc/camera.h inc/stb_image.h inc/my_texture.h src/tiny_obj_loader.cc src/tiny_obj_loader.h)

# specify the search location
set( _glfw3_HEADER_SEARCH_DIRS "/usr/local/include")
set( _glfw3_LIB_SEARCH_DIRS "/usr/local/lib")
set( glm_dirs "./glm")

set( _glfw3_ENV_ROOT $ENV{GLFW3_ROOT} )
if( NOT GLFW3_ROOT AND _glfw3_ENV_ROOT )
	set(GLFW3_ROOT ${_glfw3_ENV_ROOT} )
endif()

# Put user specified location at beginning of search
if( GLFW3_ROOT )
	list( INSERT _glfw3_HEADER_SEARCH_DIRS 0 "${GLFW3_ROOT}/include" )
	list( INSERT _glfw3_LIB_SEARCH_DIRS 0 "${GLFW3_ROOT}/lib" )
endif()

# Search for the header
FIND_PATH(GLFW3_INCLUDE_DIR "GLFW/glfw3.h" PATHS ${_glfw3_HEADER_SEARCH_DIRS})

# Search for the library
FIND_LIBRARY(GLFW3_LIBRARY NAMES libglfw.3.4.dylib PATHS ${_glfw3_LIB_SEARCH_DIRS})

#include header file directory
include_directories(${GLFW3_INCLUDE_DIR})
include_directories(${glm_dirs})
include_directories("./")

#include library file directory
link_directories(${_glfw3_LIB_SEARCH_DIRS})

#link library file
target_link_libraries(opengl-basic ${GLFW3_LIBRARY})
