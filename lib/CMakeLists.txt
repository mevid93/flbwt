#----------------------------------------------------------------------------
# Add all subdirectories containing external libraries
#----------------------------------------------------------------------------

if (BUILD_TESTS)

set(INSTALL_GTEST OFF)
add_subdirectory(googletest)

endif()