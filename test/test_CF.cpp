#include <gtest/gtest.h>
#include "CF.hpp"

class CuckooFilterTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize the CuckooFilter with a fixed seed for deterministic behavior
        srand(42);
    }

    void TearDown() override {
        // teardown code
    }
};

TEST_F(CuckooFilterTest, BasicFunctionalityTest) {
    CuckooFilter cf(100, 4, 4, 0);
    EXPECT_EQ(cf.size(), 0);

    EXPECT_EQ(cf.insert("test"), std::nullopt);
    EXPECT_EQ(cf.size(), 1);

    EXPECT_EQ(cf.contains("test"), true);
    EXPECT_EQ(cf.remove("test"), true);
    
    EXPECT_EQ(cf.size(), 0);
    EXPECT_EQ(cf.contains("test"), false);

    EXPECT_EQ(cf.isFull(), false);
}

TEST_F(CuckooFilterTest, MultipleInsertsTest) {
    // Create small cuckoo filter
    CuckooFilter cf(10, 10, 10, 0);
    EXPECT_EQ(cf.size(), 0);

    // Insert 4 items
    EXPECT_EQ(cf.insert("test1"), std::nullopt);
    EXPECT_EQ(cf.size(), 1);

    EXPECT_EQ(cf.insert("test2"), std::nullopt);
    EXPECT_EQ(cf.size(), 2);

    EXPECT_EQ(cf.insert("test3"), std::nullopt);
    EXPECT_EQ(cf.size(), 3);

    EXPECT_EQ(cf.insert("test4"), std::nullopt);
    EXPECT_EQ(cf.size(), 4);

    // Check if the items are in the filter
    EXPECT_EQ(cf.contains("test1"), true);
    EXPECT_EQ(cf.contains("test2"), true);
    EXPECT_EQ(cf.contains("test3"), true);
    EXPECT_EQ(cf.contains("test4"), true);
}

TEST_F(CuckooFilterTest, SmallerFilterTest) {
    // Create small cuckoo filter
    CuckooFilter cf(4, 4, 4, 0);
    EXPECT_EQ(cf.size(), 0);

    // Insert 4 items
    EXPECT_EQ(cf.insert("test1"), std::nullopt);
    EXPECT_EQ(cf.size(), 1);
    EXPECT_EQ(cf.contains("test1"), true);

    EXPECT_EQ(cf.insert("test2"), std::nullopt);
    EXPECT_EQ(cf.size(), 2);
    EXPECT_EQ(cf.contains("test2"), true);

    EXPECT_EQ(cf.insert("test3"), std::nullopt);
    EXPECT_EQ(cf.size(), 3);
    EXPECT_EQ(cf.contains("test3"), true);

    EXPECT_EQ(cf.insert("test4"), std::nullopt);
    EXPECT_EQ(cf.size(), 4);
    EXPECT_EQ(cf.contains("test4"), true);

    // Check if the items are in the filter
    EXPECT_EQ(cf.contains("test1"), true);
    EXPECT_EQ(cf.contains("test2"), true);
    EXPECT_EQ(cf.contains("test3"), true);
    EXPECT_EQ(cf.contains("test4"), true);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}