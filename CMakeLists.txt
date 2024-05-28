cmake_minimum_required(VERSION 3.10)
project(LDCF_Project)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(CLANG_TIDY "clang-tidy")
if(CLANG_TIDY)
    message(STATUS "clang-tidy found: ${CLANG_TIDY}")
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY}")
else()
    message(STATUS "clang-tidy not found")
endif()

# Enable testing
enable_testing()

# Add Google Test
add_subdirectory(third_party/googletest)



# Add source files
add_library(your_library
    src/CF.cpp
    src/LDCF.cpp
    src_DBF/hashfunctions.cpp
    src_DBF/dynamicbloomfilter.cpp
    src_DBF/hashfunctions.cpp
    src_DBF/countingbloomfilter.cpp
)

# Include directories
target_include_directories(your_library PUBLIC src)
target_include_directories(your_library PUBLIC src_DBF)



# Add test executable
add_executable(test_CF test/test_CF.cpp)
target_link_libraries(test_CF gtest gtest_main your_library)

# Add test executable
add_executable(test_LDCF test/test_LDCF.cpp)
target_link_libraries(test_LDCF gtest gtest_main your_library)

# Add test executable
add_executable(test_bucket test/test_bucket.cpp)
target_link_libraries(test_bucket gtest gtest_main your_library)

# Add tests to CTest
add_test(NAME TestCF COMMAND test_CF)
add_test(NAME TestLDCF COMMAND test_LDCF)
add_test(NAME TestBucket COMMAND test_bucket)

# Add benchmark executable for LDCF vs src_DBF
add_executable(DBF_vs_LDCF benchmarks/DBF_vs_LDCF.cpp)
target_link_libraries(DBF_vs_LDCF your_library)

# Add benchmark executable for LDCF vs src_DBF without fp rate
add_executable(DBF_vs_LDCF_no_fp benchmarks/DBF_vs_LDCF_no_fp.cpp)
target_link_libraries(DBF_vs_LDCF_no_fp your_library)