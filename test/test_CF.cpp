#include <gtest/gtest.h>
#include "CF.hpp"

// Example function to be tested from CF.hpp
TEST(CFTest, BasicFunctionalityTest) {
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

TEST(CFTest, AdvancedFunctionalityTest) {
    // Create small cuckoo filter
    CuckooFilter cf(4, 4, 4, 0);
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

TEST(CFTest, OverflowTest) {
    // Create small cuckoo filter
    CuckooFilter cf(4, 4, 4, 0);
    EXPECT_EQ(cf.size(), 0);

    // Insert 5 items
    EXPECT_EQ(cf.insert("test1"), std::nullopt);
    EXPECT_EQ(cf.size(), 1);

    EXPECT_EQ(cf.insert("test2"), std::nullopt);
    EXPECT_EQ(cf.size(), 2);

    EXPECT_EQ(cf.insert("test3"), std::nullopt);
    EXPECT_EQ(cf.size(), 3);

    EXPECT_EQ(cf.insert("test4"), std::nullopt);
    EXPECT_EQ(cf.size(), 4);

    // check id next insert doesnt return nullopt
    EXPECT_NE(cf.insert("test5"), std::nullopt);
    EXPECT_EQ(cf.size(), 4);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

