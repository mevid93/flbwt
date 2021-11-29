#----------------------------------------------------------------------------
# Define executable
#----------------------------------------------------------------------------
add_executable(bwt "${CMAKE_CURRENT_SOURCE_DIR}/bwt.cpp")
target_link_libraries(bwt LINK_PUBLIC flbwt)