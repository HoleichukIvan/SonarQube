#pragma once
#include <string>

class RSAHelper {
    //функції
public:
    static void GenerateKeys(const std::string& publicKeyFile, const std::string& privateKeyFile);
    static bool EncryptFileRSA(const std::string& inputFile, const std::string& outputFile, const std::string& publicKeyFile);
    static bool DecryptFileRSA(const std::string& inputFile, const std::string& outputFile, const std::string& privateKeyFile);
};
