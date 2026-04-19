#include "Lab4Window.h"
#include "RSAHelper.h"
#include "RC5.h"
#include <windows.h>
#include <commdlg.h>
#include <string>
#include <chrono>
#include <vector>

#define ID_BTN_SELECT 4001
#define ID_BTN_GENERATE 4002
#define ID_BTN_ENCRYPT_RSA 4003
#define ID_BTN_DECRYPT_RSA 4004
#define ID_BTN_ENCRYPT_RC5 4005
#define ID_BTN_DECRYPT_RC5 4006

HWND hLblFile4, hLblStatus4;
std::wstring selectedFileLab4;

std::string GetAppFolder() {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    std::string fullPath(path);
    size_t pos = fullPath.find_last_of("\\/");
    return (std::string::npos != pos) ? fullPath.substr(0, pos + 1) : "";
}

std::string WStringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    // Використовуємо CP_ACP для коректної роботи з кирилицею у шляхах Windows
    int size_needed = WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

std::wstring StringToWString(const std::string& str) {
    if (str.empty()) return std::wstring();
    // Використовуємо CP_ACP для коректної роботи з кирилицею у шляхах Windows
    int size_needed = MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

std::wstring SelectFile(HWND hwnd) {
    OPENFILENAMEW ofn;
    wchar_t szFile[260] = { 0 };
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
    ofn.lpstrFilter = L"All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameW(&ofn)) {
        return std::wstring(ofn.lpstrFile);
    }
    return L"";
}

std::wstring SaveFileAs(HWND hwnd, const wchar_t* defaultExt) {
    OPENFILENAMEW ofn;
    wchar_t szFile[260] = { 0 };
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
    ofn.lpstrFilter = L"All Files\0*.*\0";
    ofn.lpstrDefExt = defaultExt;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

    if (GetSaveFileNameW(&ofn)) {
        return std::wstring(ofn.lpstrFile);
    }
    return L"";
}

LRESULT CALLBACK Lab4WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        CreateWindowW(L"STATIC", L"Файл не вибрано",
            WS_VISIBLE | WS_CHILD | SS_LEFT,
            20, 20, 500, 20,
            hwnd, NULL, NULL, NULL);

        hLblFile4 = CreateWindowW(L"EDIT", L"",
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY | ES_AUTOHSCROLL,
            20, 45, 540, 25,
            hwnd, NULL, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Вибрати файл",
            WS_VISIBLE | WS_CHILD,
            580, 40, 150, 35,
            hwnd, (HMENU)ID_BTN_SELECT, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Згенерувати ключі (RSA 2048)",
            WS_VISIBLE | WS_CHILD,
            20, 100, 250, 40,
            hwnd, (HMENU)ID_BTN_GENERATE, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Зашифрувати RSA",
            WS_VISIBLE | WS_CHILD,
            290, 100, 200, 40,
            hwnd, (HMENU)ID_BTN_ENCRYPT_RSA, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Розшифрувати RSA",
            WS_VISIBLE | WS_CHILD,
            510, 100, 200, 40,
            hwnd, (HMENU)ID_BTN_DECRYPT_RSA, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Зашифрувати RC5",
            WS_VISIBLE | WS_CHILD,
            290, 150, 200, 40,
            hwnd, (HMENU)ID_BTN_ENCRYPT_RC5, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Розшифрувати RC5",
            WS_VISIBLE | WS_CHILD,
            510, 150, 200, 40,
            hwnd, (HMENU)ID_BTN_DECRYPT_RC5, NULL, NULL);

        CreateWindowW(L"STATIC", L"Статус виконання та час:",
            WS_VISIBLE | WS_CHILD,
            20, 210, 200, 20,
            hwnd, NULL, NULL, NULL);

        hLblStatus4 = CreateWindowW(L"EDIT", L"Очікування...",
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY,
            20, 240, 710, 100,
            hwnd, NULL, NULL, NULL);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_BTN_SELECT: {
            selectedFileLab4 = SelectFile(hwnd);
            if (!selectedFileLab4.empty()) {
                SetWindowTextW(hLblFile4, selectedFileLab4.c_str());
                SetWindowTextW(hLblStatus4, L"Файл вибрано. Готово до роботи.");
            }
            break;
        }
                          //генерація ключів
        case ID_BTN_GENERATE: {
            SetWindowTextW(hLblStatus4, L"Генерація ключів... Зачекайте.");
            std::string pubPath = GetAppFolder() + "public.key";
            std::string privPath = GetAppFolder() + "private.key";

            RSAHelper::GenerateKeys(pubPath, privPath);
            SetWindowTextW(hLblStatus4, L"Ключі public.key та private.key успішно згенеровано в папці з EXE файлом.");
            break;
        }
                            //шифрування рса
        case ID_BTN_ENCRYPT_RSA: {
            if (selectedFileLab4.empty()) {
                MessageBoxW(hwnd, L"Спочатку виберіть файл!", L"Помилка", MB_ICONERROR);
                break;
            }
            std::wstring savePath = SaveFileAs(hwnd, L"enc");
            if (!savePath.empty()) {
                SetWindowTextW(hLblStatus4, L"Шифрування RSA... Це може зайняти деякий час для великих файлів.");

                std::string pubPath = GetAppFolder() + "public.key";
                auto start = std::chrono::high_resolution_clock::now();

                bool success = RSAHelper::EncryptFileRSA(
                    WStringToString(selectedFileLab4),
                    WStringToString(savePath),
                    pubPath
                );

                auto end = std::chrono::high_resolution_clock::now();//таймер
                std::chrono::duration<double> diff = end - start;

                if (success) {
                    std::wstring msg = L"[RSA] Шифрування успішне!\nЧас виконання: " + std::to_wstring(diff.count()) + L" секунд.";
                    SetWindowTextW(hLblStatus4, msg.c_str());
                }
                else {
                    SetWindowTextW(hLblStatus4, L"Помилка шифрування RSA. Можливо, файл пошкоджений або зайнятий іншою програмою.");
                }
            }
            break;
        }

                               //розшифрування рса
        case ID_BTN_DECRYPT_RSA: {
            if (selectedFileLab4.empty()) {
                MessageBoxW(hwnd, L"Спочатку виберіть файл!", L"Помилка", MB_ICONERROR);
                break;
            }
            std::wstring savePath = SaveFileAs(hwnd, L"");
            if (!savePath.empty()) {
                SetWindowTextW(hLblStatus4, L"Розшифрування RSA... Зачекайте.");

                std::string privPath = GetAppFolder() + "private.key";
                auto start = std::chrono::high_resolution_clock::now();

                bool success = RSAHelper::DecryptFileRSA(
                    WStringToString(selectedFileLab4),
                    WStringToString(savePath),
                    privPath
                );

                auto end = std::chrono::high_resolution_clock::now();//таймер
                std::chrono::duration<double> diff = end - start;

                if (success) {
                    std::wstring msg = L"[RSA] Розшифрування успішне!\nЧас виконання: " + std::to_wstring(diff.count()) + L" секунд.";
                    SetWindowTextW(hLblStatus4, msg.c_str());
                }
                else {
                    SetWindowTextW(hLblStatus4, L"Помилка розшифрування RSA. Перевірте файл та ключ.");
                }
            }
            break;
        }
                               //шифрування рц5
        case ID_BTN_ENCRYPT_RC5: {
            if (selectedFileLab4.empty()) {
                MessageBoxW(hwnd, L"Спочатку виберіть файл!", L"Помилка", MB_ICONERROR);
                break;
            }
            std::wstring savePath = SaveFileAs(hwnd, L"enc");
            if (!savePath.empty()) {
                SetWindowTextW(hLblStatus4, L"Шифрування RC5... Зачекайте.");

                RC5Cipher rc5;
                std::vector<uint8_t> key = { 1, 2, 3, 4, 5, 6, 7, 8 };
                std::vector<uint8_t> iv = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
                rc5.expandKey(key);

                auto start = std::chrono::high_resolution_clock::now();
                bool success = rc5.encryptFile(selectedFileLab4, savePath, iv);
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> diff = end - start;

                if (success) {
                    std::wstring msg = L"[RC5] Шифрування успішне!\nЧас виконання: " + std::to_wstring(diff.count()) + L" секунд.";
                    SetWindowTextW(hLblStatus4, msg.c_str());
                }
                else {
                    SetWindowTextW(hLblStatus4, L"Помилка шифрування RC5.");
                }
            }
            break;
        }
                               //розшифрування рц5
        case ID_BTN_DECRYPT_RC5: {
            if (selectedFileLab4.empty()) {
                MessageBoxW(hwnd, L"Спочатку виберіть файл!", L"Помилка", MB_ICONERROR);
                break;
            }
            std::wstring savePath = SaveFileAs(hwnd, L"");
            if (!savePath.empty()) {
                SetWindowTextW(hLblStatus4, L"Розшифрування RC5... Зачекайте.");

                RC5Cipher rc5;
                std::vector<uint8_t> key = { 1, 2, 3, 4, 5, 6, 7, 8 };
                rc5.expandKey(key);

                auto start = std::chrono::high_resolution_clock::now();
                bool success = rc5.decryptFile(selectedFileLab4, savePath);
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> diff = end - start;

                if (success) {
                    std::wstring msg = L"[RC5] Розшифрування успішне!\nЧас виконання: " + std::to_wstring(diff.count()) + L" секунд.";
                    SetWindowTextW(hLblStatus4, msg.c_str());
                }
                else {
                    SetWindowTextW(hLblStatus4, L"Помилка розшифрування RC5.");
                }
            }
            break;
        }
        }
        break;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    default:
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void OpenLab4Window(HINSTANCE hInstance) {
    const wchar_t CLASS_NAME[] = L"Lab4WindowClass";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = Lab4WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Лабораторна робота №4 (RSA vs RC5)",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 780, 420,
        NULL,
        NULL,
        hInstance,
        NULL
    );
}