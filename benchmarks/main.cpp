#include <iostream>
#include <fstream>
#include <string>
#include "bloom_filter.h"

void loadReads(const std::string& filename, BloomFilter& bf) {
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty() && line[0] != '@') {
            bf.add(line);
        }
    }
}

void testReads(const std::string& filename, BloomFilter& bf) {
    std::ifstream file(filename);
    std::string line;
    size_t found = 0, notFound = 0;
    while (std::getline(file, line)) {
        if (!line.empty() && line[0] != '@') {
            if (bf.contains(line)) {
                found++;
            } else {
                notFound++;
            }
        }
    }
    std::cout << "Found: " << found << ", Not Found: " << notFound << std::endl;
}

int main() {
    const size_t bloomSize = 100000;
    const size_t hashCount = 5;
    BloomFilter bf(bloomSize, hashCount);

    loadReads("reads_1.fq", bf);
    testReads("reads_2.fq", bf);

    return 0;
}
