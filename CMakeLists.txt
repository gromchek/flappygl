cmake_minimum_required(VERSION 3.5)

project(fb_gl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

file( GLOB_RECURSE headers "${CMAKE_SOURCE_DIR}/*.h")
file( GLOB_RECURSE sources "${CMAKE_SOURCE_DIR}/*.cpp")

file( GLOB_RECURSE REMOVE_CMAKE "${CMAKE_SOURCE_DIR}/build/*.cpp" )
list( REMOVE_ITEM sources "${REMOVE_CMAKE}" )

find_package(OpenGL REQUIRED)

find_package(GLM)
if (GLM_FOUND)
	include_directories(${GLM_INCLUDE_DIRS})
endif()

find_package(SDL2)
if (SDL2_FOUND)
	include_directories(${SDL2_INCLUDE_DIRS})
endif()

find_package(GLEW)
if (GLEW_FOUND)
	include_directories(${GLEW_INCLUDE_DIR})
endif()

add_executable(${PROJECT_NAME} ${sources} ${headers})

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARY} ${GLEW_LIBRARY})
