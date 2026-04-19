#include "LemerGenerator.h"

LemerGenerator::LemerGenerator(uint64_t seed) {
    currentX = (seed == 0) ? 1 : (seed % m);
}

uint64_t LemerGenerator::next() {
    currentX = (a * currentX + c) % m;
    return currentX;
}

std::vector<uint64_t> LemerGenerator::generateSequence(size_t length) {
    std::vector<uint64_t> sequence;
    sequence.reserve(length);
    for (size_t i = 0; i < length; ++i) {
        sequence.push_back(next());
    }
    return sequence;
}