#pragma once
#include <cstdint>
#include <vector>

class LemerGenerator {
private:
    const uint64_t a = 8;
    const uint64_t c = 8;
    const uint64_t m = 32767;
    uint64_t currentX;

public:
    LemerGenerator(uint64_t seed);
    uint64_t next();
    std::vector<uint64_t> generateSequence(size_t length);
    uint64_t getModulus() const { return m; }
};