#pragma once
#include <string>
#include <vector>
#include <cstdint>

// Функція для тексту
std::wstring md5(const std::wstring& input);

//для файлів (Hex/Binary reader)
std::wstring md5_file(const std::wstring& filename);

class MD5_Internal {
public:
    MD5_Internal();
    void update(const std::wstring& input);
    // Метод для читання сирих байтів файлу
    void update(const uint8_t* data, size_t len);
    std::wstring hexdigest();

private:
    void process_chunk(const uint8_t chunk[64]);
    uint32_t left_rotate(uint32_t x, uint32_t amount);

    uint32_t A, B, C, D;
    uint64_t message_length;
    std::vector<uint8_t> buffer;

    static const uint32_t T[64];
    static const uint32_t S[64];
};