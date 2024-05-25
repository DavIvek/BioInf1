cmake_minimum_required(VERSION 3.10)
project(LDCF_Project)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
add_executable(test_CF test/test_CF.cpp)
target_link_libraries(test_CF gtest gtest_main your_library)

# Add test executable
add_executable(test_LDCF test/test_LDCF.cpp)
target_link_libraries(test_LDCF gtest gtest_main your_library)

# Add tests to CTest
add_test(NAME TestCF COMMAND test_CF)
add_test(NAME TestLDCF COMMAND test_LDCF)
