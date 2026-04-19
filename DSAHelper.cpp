#include "DSAHelper.h"
#include <cryptopp/dsa.h>
#include <cryptopp/osrng.h>
#include <cryptopp/files.h>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>

using namespace CryptoPP;

void DSAHelper::GenerateKeys(const std::string& publicKeyPath, const std::string& privateKeyPath) {
    AutoSeededRandomPool rng;

    // Створюємо закритий ключ (розмір 1024 біт, стандарт для звичайних лаб)
    DSA::PrivateKey privateKey;
    privateKey.GenerateRandomWithKeySize(rng, 1024);

    // Створюємо відкритий ключ з закритого
    DSA::PublicKey publicKey;
    publicKey.AssignFrom(privateKey);

    // Зберігаємо ключі у файли
    FileSink privFile(privateKeyPath.c_str());
    privateKey.Save(privFile);

    FileSink pubFile(publicKeyPath.c_str());
    publicKey.Save(pubFile);
}

bool DSAHelper::SignFile(const std::string& filePath, const std::string& privateKeyPath, const std::string& signaturePath) {
    try {
        AutoSeededRandomPool rng;

        // Завантажуємо закритий ключ
        DSA::PrivateKey privateKey;
        FileSource privFile(privateKeyPath.c_str(), true);
        privateKey.Load(privFile);

        DSA::Signer signer(privateKey);

        // Підписуємо файл (використовуємо SHA-256)
        FileSource f(filePath.c_str(), true,
            new SignerFilter(rng, signer,
                new FileSink(signaturePath.c_str())
            )
        );
        return true;
    }
    catch (...) {
        return false;
    }
}

bool DSAHelper::VerifyFile(const std::string& filePath, const std::string& publicKeyPath, const std::string& signaturePath) {
    try {
        // Завантажуємо відкритий ключ
        DSA::PublicKey publicKey;
        FileSource pubFile(publicKeyPath.c_str(), true);
        publicKey.Load(pubFile);

        DSA::Verifier verifier(publicKey);

        // Читаємо підпис із файлу
        std::string signature;
        FileSource sigFile(signaturePath.c_str(), true, new StringSink(signature));

        // Верифікуємо: об'єднуємо підпис та файл для перевірки
        bool result = false;
        SignatureVerificationFilter svf(
            verifier,
            new ArraySink((byte*)&result, sizeof(result)),
            SignatureVerificationFilter::PUT_RESULT | SignatureVerificationFilter::SIGNATURE_AT_BEGIN
        );

        svf.Put((const byte*)signature.data(), signature.size());
        FileSource f(filePath.c_str(), true, new Redirector(svf));
        svf.MessageEnd();

        return result;
    }
    catch (...) {
        return false;
    }
}