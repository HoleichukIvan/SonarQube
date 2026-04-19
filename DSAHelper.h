#pragma once
#include <string>

class DSAHelper {
public:
    // Генерація ключів (DSA 1024)
    static void GenerateKeys(const std::string& publicKeyPath, const std::string& privateKeyPath);

    // Створення цифрового підпису для файлу (зберігається в .sig)
    static bool SignFile(const std::string& filePath, const std::string& privateKeyPath, const std::string& signaturePath);

    // Верифікація цифрового підпису
    static bool VerifyFile(const std::string& filePath, const std::string& publicKeyPath, const std::string& signaturePath);
};