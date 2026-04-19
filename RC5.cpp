#include "RC5.h"
#include <cstring>
#include <vector>

using namespace std;
//підключі
void RC5Cipher::expandKey(const vector<uint8_t>& key) {
    vector<uint64_t> L(c, 0);

    for (size_t i = 0; i < b && i < key.size(); ++i) {
        L[0] |= ((uint64_t)key[i]) << (8 * i);
    }

    S[0] = 0xB7E151628AED2A6B;
    for (int i = 1; i < t; ++i) {
        S[i] = S[i - 1] + 0x9E3779B97F4A7C15;
    }

    uint64_t A = 0, B = 0;
    int i = 0, j = 0;
    int loops = 3 * t;
    //змішування
    for (int k = 0; k < loops; ++k) {
        A = S[i] = rotl(S[i] + A + B, 3);
        B = L[j] = rotl(L[j] + A + B, A + B);
        i = (i + 1) % t;
        j = (j + 1) % c;
    }
}

void RC5Cipher::encryptBlock(uint64_t& A, uint64_t& B) {
    A += S[0];
    B += S[1];
    for (int i = 1; i <= r; ++i) {
        A = rotl(A ^ B, B) + S[2 * i];
        B = rotl(B ^ A, A) + S[2 * i + 1];
    }
}

void RC5Cipher::decryptBlock(uint64_t& A, uint64_t& B) {
    for (int i = r; i >= 1; --i) {
        B = rotr(B - S[2 * i + 1], A) ^ A;
        A = rotr(A - S[2 * i], B) ^ B;
    }
    B -= S[1];
    A -= S[0];
}
//шифрування
bool RC5Cipher::encryptFile(const wstring& inPath, const wstring& outPath, const vector<uint8_t>& iv) {
    ifstream inFile(inPath, ios::binary);
    ofstream outFile(outPath, ios::binary);
    if (!inFile || !outFile) return false;
    //ділимо А і Б
    uint64_t ivA = 0, ivB = 0;
    memcpy(&ivA, iv.data(), 8);
    memcpy(&ivB, iv.data() + 8, 8);
    //вектор
    uint64_t encIvA = ivA, encIvB = ivB;
    encryptBlock(encIvA, encIvB);
    outFile.write(reinterpret_cast<const char*>(&encIvA), 8);
    outFile.write(reinterpret_cast<const char*>(&encIvB), 8);

    uint64_t prevA = encIvA, prevB = encIvB;
    uint8_t buffer[16];

    while (true) {
        inFile.read(reinterpret_cast<char*>(buffer), 16);
        int bytesRead = static_cast<int>(inFile.gcount());
        //якщо прочиталось менше 16
        if (bytesRead < 16) {
            uint8_t padValue = static_cast<uint8_t>(16 - bytesRead);
            for (int i = bytesRead; i < 16; ++i) buffer[i] = padValue;

            uint64_t A, B;
            memcpy(&A, buffer, 8);
            memcpy(&B, buffer + 8, 8);
            //СВС
            A ^= prevA; B ^= prevB;
            encryptBlock(A, B);

            outFile.write(reinterpret_cast<const char*>(&A), 8);
            outFile.write(reinterpret_cast<const char*>(&B), 8);
            break;
        }
        else {
            uint64_t A, B;
            memcpy(&A, buffer, 8);
            memcpy(&B, buffer + 8, 8);

            A ^= prevA; B ^= prevB;
            encryptBlock(A, B);

            outFile.write(reinterpret_cast<const char*>(&A), 8);
            outFile.write(reinterpret_cast<const char*>(&B), 8);
            prevA = A; prevB = B;
        }
    }
    return true;
}
//дешифрування
bool RC5Cipher::decryptFile(const wstring& inPath, const wstring& outPath) {
    ifstream inFile(inPath, ios::binary);
    if (!inFile) return false;
    //перші 16 байтів)
    uint64_t encIvA, encIvB;
    if (!inFile.read(reinterpret_cast<char*>(&encIvA), 8) || !inFile.read(reinterpret_cast<char*>(&encIvB), 8))
        return false;
    //дешифруєм ввектор
    uint64_t prevA = encIvA, prevB = encIvB;
    uint64_t decIvA = encIvA, decIvB = encIvB;
    decryptBlock(decIvA, decIvB);

    ofstream outFile(outPath, ios::binary);
    if (!outFile) return false;

    uint64_t A, B;
    uint8_t prevDecrypted[16];
    bool hasPrev = false;
    
    while (inFile.read(reinterpret_cast<char*>(&A), 8) && inFile.read(reinterpret_cast<char*>(&B), 8)) {
        if (hasPrev) {
            outFile.write(reinterpret_cast<const char*>(prevDecrypted), 16);
        }

        uint64_t currA = A, currB = B;
        decryptBlock(A, B);

        A ^= prevA; B ^= prevB;

        memcpy(prevDecrypted, &A, 8);
        memcpy(prevDecrypted + 8, &B, 8);

        prevA = currA; prevB = currB;
        hasPrev = true;
    }
    //обробка паддінгу, останній блок
    if (hasPrev) {
        uint8_t padValue = prevDecrypted[15];
        if (padValue <= 16) {
            outFile.write(reinterpret_cast<const char*>(prevDecrypted), 16 - padValue);
        }
    }

    return true;
}