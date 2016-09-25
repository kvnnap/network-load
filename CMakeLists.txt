cmake_minimum_required(VERSION 2.8)
project(network_load)

# Use C++11, enable all warnings and compile in debug mode
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
#set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_VERBOSE_MAKEFILE ON)

# Add any definitions here

# Find the system thread library and place in CMAKE_THREAD_LIBS_INIT - To link later
#find_package (Threads) -  ${CMAKE_THREAD_LIBS_INIT} to target_link_libraries
find_package(MPI REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MPI_CXX_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MPI_CXX_LINK_FLAGS}")

# Find include and library paths here

# Global include directories

# network_load target
# Find all source files in the below directory automatically
file(GLOB_RECURSE SOURCE_FILES
     Application/*.cpp
     Sampler/*.cpp
)

add_executable(network_load main.cpp ${SOURCE_FILES})
target_include_directories(network_load PUBLIC . ${MPI_CXX_INCLUDE_PATH})
target_link_libraries (network_load ${MPI_CXX_LIBRARIES})