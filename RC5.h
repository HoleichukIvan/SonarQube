#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <fstream>
#include <stdexcept>

class RC5Cipher {
private:
    static const int w = 64; //розмір слова в бітах
    static const int r = 12; //кількість раундів
    static const int b = 8; //довджина ключа
    static const int t = 2 * r + 2;
    static const int c = 1; //кількість слів у ключі

    uint64_t S[t]; //підключі

    inline uint64_t rotl(uint64_t val, uint64_t shift) {
        shift &= 63; //%64
        return (shift == 0) ? val : (val << shift) | (val >> (64 - shift));
    }
    //зсув врпаво
    inline uint64_t rotr(uint64_t val, uint64_t shift) {
        shift &= 63;
        return (shift == 0) ? val : (val >> shift) | (val << (64 - shift));
    }
    //шифрування і дешифрування
    void encryptBlock(uint64_t& A, uint64_t& B);
    void decryptBlock(uint64_t& A, uint64_t& B);

public:
    RC5Cipher() {
        for (int i = 0; i < t; i++) S[i] = 0;
    }

    void expandKey(const std::vector<uint8_t>& key);

    bool encryptFile(const std::wstring& inPath, const std::wstring& outPath, const std::vector<uint8_t>& iv);
    bool decryptFile(const std::wstring& inPath, const std::wstring& outPath);
};