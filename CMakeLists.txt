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

# Find OpenSSL
find_package(OpenSSL REQUIRED)

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
target_include_directories(your_library PUBLIC ${OPENSSL_INCLUDE_DIR})

# Link OpenSSL libraries
target_link_libraries(your_library PUBLIC OpenSSL::SSL OpenSSL::Crypto)

# Add test executable
add_executable(test_CF test/test_CF.cpp)
target_link_libraries(test_CF gtest gtest_main your_library)

# Add test executable
add_executable(test_LDCF test/test_LDCF.cpp)
target_link_libraries(test_LDCF gtest gtest_main your_library)

# Add tests to CTest
add_test(NAME TestCF COMMAND test_CF)
add_test(NAME TestLDCF COMMAND test_LDCF)

# Add benchmark executable for Bloom filter vs LDCF
add_executable(benchmark_bloom_vs_ldcf benchmarks/bloom_vs_ldcf.cpp)
target_link_libraries(benchmark_bloom_vs_ldcf your_library)

# Add benchmark executable for LDCF vs src_DBF
add_executable(DBF_vs_LDCF benchmarks/DBF_vs_LDCF.cpp)
target_link_libraries(DBF_vs_LDCF your_library)