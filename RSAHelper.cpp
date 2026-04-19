#include "RSAHelper.h"
#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>
#include <cryptopp/files.h>
#include <cryptopp/filters.h>
#include <fstream>
#include <vector>


void RSAHelper::GenerateKeys(const std::string& publicKeyFile, const std::string& privateKeyFile) {
    CryptoPP::AutoSeededRandomPool rng;
    //згенеровані числа
    CryptoPP::InvertibleRSAFunction parameters;

    parameters.GenerateRandomWithKeySize(rng, 2048);//генерація
    //формуємо
    CryptoPP::RSA::PrivateKey privateKey(parameters);
    CryptoPP::RSA::PublicKey publicKey(parameters);
    //зберігаємо ключі
    CryptoPP::FileSink privFile(privateKeyFile.c_str());
    privateKey.DEREncode(privFile); 
    privFile.MessageEnd();

    CryptoPP::FileSink pubFile(publicKeyFile.c_str());
    publicKey.DEREncode(pubFile);
    pubFile.MessageEnd();
}


bool RSAHelper::EncryptFileRSA(const std::string& inputFile, const std::string& outputFile, const std::string& publicKeyFile) {
    //щоб програма не падала
    try {
        CryptoPP::AutoSeededRandomPool rng;
       
        CryptoPP::FileSource pubFile(publicKeyFile.c_str(), true);
        CryptoPP::RSA::PublicKey publicKey;
        publicKey.BERDecode(pubFile);//декодуєм бінарні дані
        //шифрувальник OAEP
        CryptoPP::RSAES_OAEP_SHA_Encryptor encryptor(publicKey);
        //214
        size_t maxPlaintextLength = encryptor.FixedMaxPlaintextLength();

        std::ifstream in(inputFile, std::ios::binary);
        std::ofstream out(outputFile, std::ios::binary);

        if (!in.is_open() || !out.is_open()) return false;

        std::vector<CryptoPP::byte> buffer(maxPlaintextLength);

        while (in.read(reinterpret_cast<char*>(buffer.data()), maxPlaintextLength) || in.gcount() > 0) {
            size_t bytesRead = in.gcount();//скільки прочитали
            std::string cipherText;
            //Шифруємо
            CryptoPP::StringSource ss(buffer.data(), bytesRead, true,
                new CryptoPP::PK_EncryptorFilter(rng, encryptor,
                    new CryptoPP::StringSink(cipherText)
                )
            );
            out.write(cipherText.data(), cipherText.size());
        }
        return true;
    }
    catch (const CryptoPP::Exception& e) {
        return false;
    }
}

bool RSAHelper::DecryptFileRSA(const std::string& inputFile, const std::string& outputFile, const std::string& privateKeyFile) {
    try {
        CryptoPP::AutoSeededRandomPool rng;

        CryptoPP::FileSource privFile(privateKeyFile.c_str(), true);
        CryptoPP::RSA::PrivateKey privateKey;
        privateKey.BERDecode(privFile);
        //дешифратор
        CryptoPP::RSAES_OAEP_SHA_Decryptor decryptor(privateKey);
        //256 байт
        size_t ciphertextLength = decryptor.FixedCiphertextLength();

        std::ifstream in(inputFile, std::ios::binary);
        std::ofstream out(outputFile, std::ios::binary);

        if (!in.is_open() || !out.is_open()) return false;

        std::vector<CryptoPP::byte> buffer(ciphertextLength);

        while (in.read(reinterpret_cast<char*>(buffer.data()), ciphertextLength)) {
            size_t bytesRead = in.gcount();
            //якщо не 256
            if (bytesRead != ciphertextLength) break;
            //дешифруємо
            std::string recoveredText;
            CryptoPP::StringSource ss(buffer.data(), bytesRead, true,
                new CryptoPP::PK_DecryptorFilter(rng, decryptor,
                    new CryptoPP::StringSink(recoveredText)
                )
            );
            out.write(recoveredText.data(), recoveredText.size());
        }
        return true;
    }
    catch (const CryptoPP::Exception& e) {
        return false;
    }
}