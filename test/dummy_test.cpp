// tests/test_main.cpp
#include <gtest/gtest.h>
#include "CF.hpp"
#include "LDCF.hpp"

// Example function to be tested from CF.hpp
int Add(int a, int b) {
    return a + b;
}

// Example test case for CF
TEST(CFTest, HandlesPositiveInput) {
    EXPECT_EQ(Add(1, 2), 3);
    EXPECT_EQ(Add(5, 6), 11);
}

// Example function to be tested from LDCF.hpp
int Multiply(int a, int b) {
    return a * b;
}

// Example test case for LDCF
TEST(LDCFTest, HandlesPositiveInput) {
    EXPECT_EQ(Multiply(2, 3), 6);
    EXPECT_EQ(Multiply(5, 6), 30);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
