#include "MD5.h"
#include <sstream>
#include <iomanip>
#include <cstring>
#include <fstream> 

using namespace std;

MD5_Internal::MD5_Internal() {
    A = 0x67452301;
    B = 0xEFCDAB89;
    C = 0x98BADCFE;
    D = 0x10325476;
    message_length = 0;
}

uint32_t MD5_Internal::left_rotate(uint32_t x, uint32_t amount) {
    return (x << amount) | (x >> (32 - amount));
}

void MD5_Internal::process_chunk(const uint8_t chunk[64]) {
    uint32_t X[16];
    memcpy(X, chunk, 64);

    uint32_t a = A, b = B, c = C, d = D;

    for (int j = 0; j < 64; j++) {
        uint32_t F, g;
        if (j <= 15) {
            F = (b & c) | ((~b) & d);
            g = j;
        }
        else if (j <= 31) {
            F = (d & b) | ((~d) & c);
            g = (5 * j + 1) % 16;
        }
        else if (j <= 47) {
            F = b ^ c ^ d;
            g = (3 * j + 5) % 16;
        }
        else {
            F = c ^ (b | (~d));
            g = (7 * j) % 16;
        }

        F = F + a + T[j] + X[g];
        uint32_t temp = b + left_rotate(F, S[j]);
        a = d; d = c; c = b; b = temp;
    }

    A += a; B += b; C += c; D += d;
}

void MD5_Internal::update(const wstring& input) {
    string data;
    for (wchar_t wc : input) data += (char)(wc & 0xFF);

    const uint8_t* ptr = (const uint8_t*)data.c_str();
    size_t len = data.length();
    message_length += len;

    for (size_t i = 0; i < len; i++) {
        buffer.push_back(ptr[i]);
        if (buffer.size() == 64) {
            process_chunk(buffer.data());
            buffer.clear();
        }
    }
}
//hex reader
void MD5_Internal::update(const uint8_t* data, size_t len) {
    message_length += len;
    for (size_t i = 0; i < len; i++) {
        buffer.push_back(data[i]);
        if (buffer.size() == 64) {
            process_chunk(buffer.data());
            buffer.clear();
        }
    }
}

wstring MD5_Internal::hexdigest() {
    uint64_t bit_len = message_length * 8;

    buffer.push_back(0x80);
    while (buffer.size() % 64 != 56) {
        buffer.push_back(0x00);
    }

    for (int i = 0; i < 8; i++) {
        buffer.push_back((uint8_t)(bit_len >> (i * 8)));
    }

    for (size_t i = 0; i < buffer.size(); i += 64) {
        process_chunk(buffer.data() + i);
    }

    wstringstream ss;
    uint32_t regs[4] = { A, B, C, D };
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            ss << hex << setw(2) << setfill(L'0') << (int)((regs[i] >> (j * 8)) & 0xFF);
        }
    }
    return ss.str();
}

const uint32_t MD5_Internal::T[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

const uint32_t MD5_Internal::S[64] = {
    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
};

wstring md5(const wstring& input) {
    MD5_Internal hasher;
    hasher.update(input);
    return hasher.hexdigest();
}
//hex reader
wstring md5_file(const wstring& filename) {
    MD5_Internal hasher;
    ifstream file(filename, ios::binary);
    if (!file) return L"File open error!";

    char buffer[4096];
    while (file) {
        file.read(buffer, sizeof(buffer));
        size_t count = file.gcount();
        if (count > 0) {
            hasher.update(reinterpret_cast<const uint8_t*>(buffer), count);
        }
    }
    return hasher.hexdigest();
}