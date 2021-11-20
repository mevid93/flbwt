# Set the minimum version of CMake that is required
cmake_minimum_required(VERSION 3.5)

# set the project name
project(flbwt)

# enable debug information and all warnings
set(CMAKE_CXX_FLAGS "-g -Wall")

# set cpp standard
set(CMAKE_CXX_STANDARD 11)

#----------------------------------------------------------------------------
# Define project sources and includes
#----------------------------------------------------------------------------
set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR}/include)
include_directories(${COMMON_INCLUDES})
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

#----------------------------------------------------------------------------
# Define libraries and executables
#----------------------------------------------------------------------------
add_library(project1_lib ${SRC_FILES})
# add_executable(project1 ${PROJECT_SOURCE_DIR}/examples/bwt)
# target_link_libraries(project1 project1_lib)


#----------------------------------------------------------------------------
# Testing (cmake -DBUILD_TESTS=ON ..)
#----------------------------------------------------------------------------

# Options. Turn on with 'cmake -DBUILD_TESTS=ON'.
option(BUILD_TESTS "Build all tests." OFF) # Makes boolean 'test' available.

if (BUILD_TESTS)

  # Google test framework
  include(FetchContent)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.11.0
  )
  FetchContent_MakeAvailable(googletest)

  enable_testing()

  file(GLOB TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/test/*.cpp)
  add_executable(UnitTests ${TEST_SRC_FILES})

  # Standard linking to gtest stuff.
  target_link_libraries(UnitTests gtest gtest_main)

  # Extra linking for the project.
  target_link_libraries(UnitTests project1_lib)

  include(GoogleTest)
  gtest_discover_tests(UnitTests)

endif()