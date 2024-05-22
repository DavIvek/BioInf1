cmake_minimum_required(VERSION 3.10)
project(LDCF_Project)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Enable testing
enable_testing()

# Add Google Test
add_subdirectory(third_party/googletest)

# Add source files
add_library(your_library
    src/CF.cpp
    src/LDCF.cpp
)

# Include directories
target_include_directories(your_library PUBLIC src)

# Add test executable
add_executable(your_tests test/dummy_test.cpp)
target_link_libraries(your_tests gtest gtest_main your_library)

# Add tests to CTest
add_test(NAME YourTestName COMMAND your_tests)
