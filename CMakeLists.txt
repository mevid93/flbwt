#----------------------------------------------------------------------------
# Set basic properties (project name and compiler)
#----------------------------------------------------------------------------
# Set the minimum version of CMake that is required
cmake_minimum_required(VERSION 3.5)

# set the project name
project(flbwt)

# enable debug information and all warnings
set(CMAKE_CXX_FLAGS "-g -Wall")

# set cpp standard
set(CMAKE_CXX_STANDARD 11)


#----------------------------------------------------------------------------
# External libraries
#----------------------------------------------------------------------------
add_subdirectory(lib)


#----------------------------------------------------------------------------
# Define flbwt library
#----------------------------------------------------------------------------
file(GLOB SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_library(flbwt "${SRC_FILES}")
target_include_directories(flbwt PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

#----------------------------------------------------------------------------
# Add all the other subdirectories containing a CMakeLists.txt
#----------------------------------------------------------------------------
add_subdirectory(examples)
enable_testing()
add_subdirectory(test)