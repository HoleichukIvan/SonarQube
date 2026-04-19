#include "Lab5Window.h"
#include "DSAHelper.h"
#include <commdlg.h>
#include <string>

// Копіюємо необхідні хелпери з Lab4Window (в ідеалі їх варто винести в Utils.h)
extern std::string GetAppFolder();
extern std::string WStringToString(const std::wstring& wstr);
extern std::wstring StringToWString(const std::string& str);
extern std::wstring SelectFile(HWND hwnd);
extern std::wstring SaveFileAs(HWND hwnd, const wchar_t* defaultExt);

#define ID_BTN_SELECT_FILE 5001
#define ID_BTN_GENERATE_DSA 5002
#define ID_BTN_SIGN 5003
#define ID_BTN_VERIFY 5004
#define ID_BTN_SELECT_SIG 5005

HWND hLblFile5, hLblSig5, hLblStatus5;
std::wstring selectedFileLab5;
std::wstring selectedSigLab5;

LRESULT CALLBACK Lab5WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        CreateWindowW(L"STATIC", L"Файл для підпису/перевірки:", WS_VISIBLE | WS_CHILD | SS_LEFT, 20, 20, 500, 20, hwnd, NULL, NULL, NULL);
        hLblFile5 = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY | ES_AUTOHSCROLL, 20, 45, 540, 25, hwnd, NULL, NULL, NULL);
        CreateWindowW(L"BUTTON", L"Вибрати файл", WS_VISIBLE | WS_CHILD, 580, 40, 150, 35, hwnd, (HMENU)ID_BTN_SELECT_FILE, NULL, NULL);

        CreateWindowW(L"STATIC", L"Файл підпису (.sig) для перевірки:", WS_VISIBLE | WS_CHILD | SS_LEFT, 20, 80, 500, 20, hwnd, NULL, NULL, NULL);
        hLblSig5 = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY | ES_AUTOHSCROLL, 20, 105, 540, 25, hwnd, NULL, NULL, NULL);
        CreateWindowW(L"BUTTON", L"Вибрати підпис", WS_VISIBLE | WS_CHILD, 580, 100, 150, 35, hwnd, (HMENU)ID_BTN_SELECT_SIG, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Згенерувати ключі (DSA)", WS_VISIBLE | WS_CHILD, 20, 160, 220, 40, hwnd, (HMENU)ID_BTN_GENERATE_DSA, NULL, NULL);
        CreateWindowW(L"BUTTON", L"Підписати файл (Створити .sig)", WS_VISIBLE | WS_CHILD, 260, 160, 230, 40, hwnd, (HMENU)ID_BTN_SIGN, NULL, NULL);
        CreateWindowW(L"BUTTON", L"Перевірити підпис", WS_VISIBLE | WS_CHILD, 510, 160, 220, 40, hwnd, (HMENU)ID_BTN_VERIFY, NULL, NULL);

        CreateWindowW(L"STATIC", L"Статус виконання:", WS_VISIBLE | WS_CHILD, 20, 220, 200, 20, hwnd, NULL, NULL, NULL);
        hLblStatus5 = CreateWindowW(L"EDIT", L"Очікування...", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY, 20, 250, 710, 100, hwnd, NULL, NULL, NULL);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_BTN_SELECT_FILE:
            selectedFileLab5 = SelectFile(hwnd);
            if (!selectedFileLab5.empty()) {
                SetWindowTextW(hLblFile5, selectedFileLab5.c_str());
            }
            break;

        case ID_BTN_SELECT_SIG:
            selectedSigLab5 = SelectFile(hwnd);
            if (!selectedSigLab5.empty()) {
                SetWindowTextW(hLblSig5, selectedSigLab5.c_str());
            }
            break;

        case ID_BTN_GENERATE_DSA: {
            std::string pubPath = GetAppFolder() + "dsa_public.key";
            std::string privPath = GetAppFolder() + "dsa_private.key";
            DSAHelper::GenerateKeys(pubPath, privPath);
            SetWindowTextW(hLblStatus5, L"Ключі dsa_public.key та dsa_private.key успішно згенеровано.");
            break;
        }

        case ID_BTN_SIGN: {
            if (selectedFileLab5.empty()) {
                MessageBoxW(hwnd, L"Виберіть файл для підпису!", L"Помилка", MB_ICONERROR);
                break;
            }
            std::wstring savePath = SaveFileAs(hwnd, L"sig");
            if (!savePath.empty()) {
                std::string privPath = GetAppFolder() + "dsa_private.key";
                bool success = DSAHelper::SignFile(WStringToString(selectedFileLab5), privPath, WStringToString(savePath));
                if (success) {
                    SetWindowTextW(hLblStatus5, L"[DSS] Файл успішно підписано. Підпис збережено.");
                }
                else {
                    SetWindowTextW(hLblStatus5, L"[DSS] Помилка створення підпису. Перевірте наявність dsa_private.key.");
                }
            }
            break;
        }

        case ID_BTN_VERIFY: {
            if (selectedFileLab5.empty() || selectedSigLab5.empty()) {
                MessageBoxW(hwnd, L"Потрібно вибрати як основний файл, так і файл підпису (.sig)!", L"Помилка", MB_ICONWARNING);
                break;
            }
            std::string pubPath = GetAppFolder() + "dsa_public.key";
            bool isValid = DSAHelper::VerifyFile(WStringToString(selectedFileLab5), pubPath, WStringToString(selectedSigLab5));
            if (isValid) {
                SetWindowTextW(hLblStatus5, L"[DSS] УСПІХ! Цифровий підпис ДІЙСНИЙ. Файл не був змінений.");
            }
            else {
                SetWindowTextW(hLblStatus5, L"[DSS] УВАГА! Цифровий підпис НЕ ДІЙСНИЙ або файл пошкоджено.");
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

void OpenLab5Window(HINSTANCE hInstance) {
    const wchar_t CLASS_NAME[] = L"Lab5WindowClass";
    WNDCLASSW wc = {};
    wc.lpfnWndProc = Lab5WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassW(&wc);

    CreateWindowExW(0, CLASS_NAME, L"Лабораторна робота №5 (DSS / DSA Підпис)",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 780, 420,
        NULL, NULL, hInstance, NULL);
}