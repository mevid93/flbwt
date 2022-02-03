#----------------------------------------------------------------------------
# Testing (cmake -DBUILD_TESTS=ON ..)
#----------------------------------------------------------------------------

# Options. Turn on with 'cmake -DBUILD_TESTS=ON'.
option(BUILD_TESTS "Build all tests." OFF) # Makes boolean 'test' available.

if (BUILD_TESTS)

    include(GoogleTest)

    file(GLOB TEST_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
    add_executable(UnitTests "${TEST_SRC_FILES}")

    # Standard linking to gtest stuff.
    target_link_libraries(UnitTests gtest gtest_main)

    # Extra linking for the project.
    target_link_libraries(UnitTests flbwt)

    gtest_discover_tests(UnitTests)

endif()