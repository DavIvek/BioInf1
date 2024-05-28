#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <gtest/gtest.h>
#include "LDCF.hpp"

class LogarithmicDynamicCuckooFilterTest : public ::testing::Test {
protected:
    void SetUp() override {
        srand(42);
    }

    void TearDown() override {
    }

    // Helper function to load reads from a FASTQ file
    void loadReadsFromFastq(const std::string& filename, std::vector<std::string>& reads) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Unable to open file: " + filename);
        }
        std::string line;
        int line_number = 0;
        while (std::getline(file, line)) {
            if (line_number % 4 == 1) {
                reads.push_back(line);
            }
            line_number++;
        }
    }
};

TEST_F(LogarithmicDynamicCuckooFilterTest, RealDataTest) {
    std::vector<std::string> reads1, reads2;
    loadReadsFromFastq("reads_1.fq", reads1);
    loadReadsFromFastq("reads_2.fq", reads2);
    LogarithmicDynamicCuckooFilter ldCF(0.1, 4, 1);

    // Insert reads from the first file and check their presence
    for (const auto& read : reads1) {
        ldCF.insert(read);
        EXPECT_EQ(ldCF.contains(read), true);
    }

    // Check if reads from the first file are present
    for (const auto& read : reads1) {
        EXPECT_EQ(ldCF.contains(read), true);
    }

    // Insert reads from the second file and check their presence
    for (const auto& read : reads2) {
        ldCF.insert(read);
        EXPECT_EQ(ldCF.contains(read), true);
    }

    // Check if reads from the second file are present
    for (const auto& read : reads2) {
        EXPECT_EQ(ldCF.contains(read), true);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();