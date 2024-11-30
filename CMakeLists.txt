cmake_minimum_required(VERSION 3.10)
project(qr)

# Use C++17
set(CMAKE_CXX_STANDARD 17)

# Download GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

if (WIN32)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()
FetchContent_MakeAvailable(googletest)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Build the executable
add_executable(qr main.cc qr.cc)

# Build the test executable
add_executable(qr_test qr_test.cc qr.cc)  # Add qr.cc here to link with qr_test

# Link GoogleTest libraries to the test executable
target_link_libraries(qr_test gtest gtest_main)

# Enable testing
enable_testing()

# Add test target
add_test(NAME QRTest COMMAND qr_test)