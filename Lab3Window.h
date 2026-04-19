#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <bitset>
#include "RC5.h"
#include "LemerService.h"
#include "MD5.h"

using namespace std;

#define ID_BTN_SELECT_IN  3001
#define ID_BTN_SELECT_OUT 3002
#define ID_BTN_ENCRYPT    3003
#define ID_BTN_DECRYPT    3004
#define ID_EDIT_PASS      3005
#define ID_EDIT_HEX       3006

HWND hPathIn, hPathOut, hPass, hHexViewer;
wstring currentFileIn = L"";
wstring currentFileOut = L"";

//перетворюємо 64-бітне число у текст
wstring MemoryDebug(uint64_t value, const wstring& name) {
    wstringstream wss;
    wss << L"--- Memory Debug: " << name << L" ---\r\n";
    wss << L"[INT]: " << dec << value << L"\r\n";
    wss << L"[HEX]: 0x" << hex << uppercase << setw(16) << setfill(L'0') << value << L"\r\n";

    bitset<64> bits(value);
    string bitStr = bits.to_string();
    wss << L"[BIN]: ";
    for (int i = 0; i < 64; ++i) {
        wss << (wchar_t)bitStr[i];
        if ((i + 1) % 8 == 0 && i != 63) wss << L" ";
    }
    wss << L"\r\n";

    return wss.str();
}

wstring GenerateHexDump(const wstring& filepath) {
    ifstream file(filepath, ios::binary);
    if (!file) return L"Не вдалося відкрити файл для читання.";

    uint8_t buf[256];
    file.read(reinterpret_cast<char*>(buf), 256);
    int read = static_cast<int>(file.gcount());

    wstringstream wss;
    wss << L"--- Дамп перших " << read << L" байт ---\r\n";
    for (int i = 0; i < read; i++) {
        wss << hex << setw(2) << setfill(L'0') << uppercase << (int)buf[i] << L" ";
        if ((i + 1) % 16 == 0) wss << L"\r\n";
    }
    return wss.str();
}
// пароль 8 байт
vector<uint8_t> GetKeyFromPassword(const wstring& pass) {
    wstring hash = md5(pass);
    //масив для ключа
    vector<uint8_t> key(8, 0);

    for (size_t i = 0; i < 8; ++i) {
        wstring byteStr = hash.substr(i * 2, 2);
        key[i] = static_cast<uint8_t>(stoi(byteStr, nullptr, 16));// Перетворення MD5 (hex) у перші 8 байтів ключа
    }

    return key;
}

LRESULT CALLBACK Lab3WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        CreateWindow(L"STATIC", L"Вхідний файл:", WS_VISIBLE | WS_CHILD, 20, 20, 100, 20, hwnd, NULL, NULL, NULL);
        hPathIn = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY, 130, 20, 400, 25, hwnd, NULL, NULL, NULL);
        CreateWindow(L"BUTTON", L"Вибрати...", WS_VISIBLE | WS_CHILD, 540, 20, 100, 25, hwnd, (HMENU)ID_BTN_SELECT_IN, NULL, NULL);

        CreateWindow(L"STATIC", L"Вихідний файл:", WS_VISIBLE | WS_CHILD, 20, 60, 100, 20, hwnd, NULL, NULL, NULL);
        hPathOut = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY, 130, 60, 400, 25, hwnd, NULL, NULL, NULL);
        CreateWindow(L"BUTTON", L"Вибрати...", WS_VISIBLE | WS_CHILD, 540, 60, 100, 25, hwnd, (HMENU)ID_BTN_SELECT_OUT, NULL, NULL);

        CreateWindow(L"STATIC", L"Пароль:", WS_VISIBLE | WS_CHILD, 20, 100, 100, 20, hwnd, NULL, NULL, NULL);
        hPass = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 130, 100, 200, 25, hwnd, (HMENU)ID_EDIT_PASS, NULL, NULL);

        CreateWindow(L"BUTTON", L"Зашифрувати", WS_VISIBLE | WS_CHILD, 130, 140, 120, 30, hwnd, (HMENU)ID_BTN_ENCRYPT, NULL, NULL);
        CreateWindow(L"BUTTON", L"Дешифрувати", WS_VISIBLE | WS_CHILD, 260, 140, 120, 30, hwnd, (HMENU)ID_BTN_DECRYPT, NULL, NULL);

        hHexViewer = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | WS_VSCROLL | ES_READONLY, 20, 190, 620, 250, hwnd, (HMENU)ID_EDIT_HEX, NULL, NULL);
        break;

    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        if (wmId == ID_BTN_SELECT_IN || wmId == ID_BTN_SELECT_OUT) {
            OPENFILENAME ofn;
            wchar_t szFile[260] = { 0 };
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = L"Усі файли\0*.*\0";
            ofn.nFilterIndex = 1;

            if (wmId == ID_BTN_SELECT_IN) {
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
                if (GetOpenFileName(&ofn)) {
                    currentFileIn = szFile;
                    SetWindowText(hPathIn, currentFileIn.c_str());
                    SetWindowText(hHexViewer, GenerateHexDump(currentFileIn).c_str());
                }
            }
            else {
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
                if (GetSaveFileName(&ofn)) {
                    currentFileOut = szFile;
                    SetWindowText(hPathOut, currentFileOut.c_str());
                }
            }
        }
        else if (wmId == ID_BTN_ENCRYPT || wmId == ID_BTN_DECRYPT) {
            wchar_t passBuf[256];
            GetWindowText(hPass, passBuf, 256);
            if (currentFileIn.empty() || currentFileOut.empty() || wcslen(passBuf) == 0) {
                MessageBox(hwnd, L"Виберіть файли та введіть пароль!", L"Помилка", MB_OK | MB_ICONERROR);
                break;
            }

            RC5Cipher cipher;
            cipher.expandKey(GetKeyFromPassword(passBuf));

            bool success = false;
            wstring tempFile = L"temp_magic.bin";

            if (wmId == ID_BTN_ENCRYPT) {
                //Створюємо тимчасовий файл 
                ifstream inFile(currentFileIn, ios::binary);
                ofstream tFile(tempFile, ios::binary);
                tFile.write("RC5_OK!!", 8); // маркер
                tFile << inFile.rdbuf();    
                inFile.close();
                tFile.close();

                //Генеруємо вектор ініціалізації
                uint64_t seed = GetTickCount64();
                wstring lemerOut = LemerService::RunLab(seed, 16);
                vector<uint8_t> iv(16, 0);
                for (int i = 0; i < 16 && i < lemerOut.length(); i++) {
                    iv[i] = static_cast<uint8_t>(lemerOut[i]);
                }

                
                uint64_t debugValue = 0;
                memcpy(&debugValue, iv.data(), 8);
                wstring debugMsg = MemoryDebug(debugValue, L"Перше слово вектора IV");
                MessageBox(hwnd, debugMsg.c_str(), L"Memory Debug", MB_OK | MB_ICONINFORMATION);

                
                success = cipher.encryptFile(tempFile, currentFileOut, iv);

               
                _wremove(tempFile.c_str());
            }
            else {
                //Дешифруємо файл у тимчасовий файл
                bool decrypted = cipher.decryptFile(currentFileIn, tempFile);

                if (decrypted) {
                    ifstream tFile(tempFile, ios::binary);
                    char checkBuf[8] = { 0 };
                    tFile.read(checkBuf, 8); 

                    
                    if (tFile.gcount() == 8 && memcmp(checkBuf, "RC5_OK!!", 8) == 0) {
                       
                        ofstream outFile(currentFileOut, ios::binary);
                        outFile << tFile.rdbuf();
                        outFile.close();
                        success = true;
                    }
                    tFile.close();
                }

               
                _wremove(tempFile.c_str());
            }

            
            if (success) {
                MessageBox(hwnd, L"Операція успішна!", L"Готово", MB_OK);
                SetWindowText(hHexViewer, GenerateHexDump(currentFileOut).c_str());
            }
            else {
                if (wmId == ID_BTN_DECRYPT) {
                    // Якщо успіху немає
                    MessageBox(hwnd, L"Неправильний пароль! Файл не розшифровано.", L"Відмовлено в доступі", MB_OK | MB_ICONWARNING);
                }
                else {
                    MessageBox(hwnd, L"Помилка роботи з файлом!", L"Помилка", MB_OK | MB_ICONERROR);
                }
            }
        }
        break;
    }
    case WM_CLOSE: DestroyWindow(hwnd); break;
    default: return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void OpenLab3Window(HINSTANCE hInstance) {
    const wchar_t CLASS_NAME[] = L"Lab3WindowClass";
    WNDCLASS wc = {};
    wc.lpfnWndProc = Lab3WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"Лабораторна робота №3 - RC5",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 680, 500,
        NULL, NULL, hInstance, NULL);
}