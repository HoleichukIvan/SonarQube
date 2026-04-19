#include <windows.h>
#include <string>
#include "LemerService.h"
#include "Lab2Window.h"
#include "Lab3Window.h"
#include "Lab4Window.h" 
#include "Lab5Window.h" // ДОДАНО: Підключення заголовного файлу ЛР №5

using namespace std;

#define ID_LAB1 101
#define ID_LAB2 102
#define ID_LAB3 103
#define ID_LAB4 104
#define ID_LAB5 105
#define ID_RUN  200
#define ID_SEED 300
#define ID_COUNT 301
#define ID_OUTPUT 400

HWND hSeed, hCount, hOutput;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int application(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"MainWindow";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Система захисту інформації",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 900, 600,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

int main()
{
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    return application(hInstance, nullptr, GetCommandLineW(), SW_SHOWDEFAULT);
}

void RunLab1()
{
    wchar_t seedStr[50], countStr[50];
    GetWindowText(hSeed, seedStr, 50);
    GetWindowText(hCount, countStr, 50);

    uint64_t seed = _wtoi(seedStr);
    int count = _wtoi(countStr);

    if (seed <= 0 || count <= 0 || count > 1000)
    {
        MessageBox(NULL, L"Невірні дані!", L"Помилка", MB_OK);
        return;
    }

    wstring result = LemerService::RunLab(seed, count);

    SetWindowText(hOutput, result.c_str());

    MessageBox(NULL, L"Результати збережено у results.txt", L"Готово", MB_OK);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        CreateWindow(L"BUTTON", L"Лабораторна робота №1",
            WS_VISIBLE | WS_CHILD,
            20, 20, 220, 40,
            hwnd, (HMENU)ID_LAB1, NULL, NULL);

        CreateWindow(L"BUTTON", L"Лабораторна робота №2",
            WS_VISIBLE | WS_CHILD,
            20, 70, 220, 40,
            hwnd, (HMENU)ID_LAB2, NULL, NULL);

        CreateWindow(L"BUTTON", L"Лабораторна робота №3",
            WS_VISIBLE | WS_CHILD,
            20, 120, 220, 40,
            hwnd, (HMENU)ID_LAB3, NULL, NULL);

        CreateWindow(L"BUTTON", L"Лабораторна робота №4",
            WS_VISIBLE | WS_CHILD,
            20, 170, 220, 40,
            hwnd, (HMENU)ID_LAB4, NULL, NULL);

        // ЗМІНЕНО: Видалено прапорець WS_DISABLED, щоб кнопка стала активною
        CreateWindow(L"BUTTON", L"Лабораторна робота №5",
            WS_VISIBLE | WS_CHILD,
            20, 220, 220, 40,
            hwnd, (HMENU)ID_LAB5, NULL, NULL);

        CreateWindow(L"STATIC", L"Seed:",
            WS_VISIBLE | WS_CHILD,
            280, 20, 50, 20,
            hwnd, NULL, NULL, NULL);

        hSeed = CreateWindow(L"EDIT", L"",
            WS_VISIBLE | WS_CHILD | WS_BORDER,
            340, 20, 100, 25,
            hwnd, (HMENU)ID_SEED, NULL, NULL);

        CreateWindow(L"STATIC", L"Count:",
            WS_VISIBLE | WS_CHILD,
            460, 20, 60, 20,
            hwnd, NULL, NULL, NULL);

        hCount = CreateWindow(L"EDIT", L"",
            WS_VISIBLE | WS_CHILD | WS_BORDER,
            520, 20, 100, 25,
            hwnd, (HMENU)ID_COUNT, NULL, NULL);

        CreateWindow(L"BUTTON", L"Запустити",
            WS_VISIBLE | WS_CHILD,
            650, 18, 120, 30,
            hwnd, (HMENU)ID_RUN, NULL, NULL);

        hOutput = CreateWindow(L"EDIT", L"",
            WS_VISIBLE | WS_CHILD | WS_BORDER |
            ES_MULTILINE | WS_VSCROLL | ES_AUTOVSCROLL,
            280, 70, 580, 450,
            hwnd, (HMENU)ID_OUTPUT, NULL, NULL);

        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_RUN:
            RunLab1();
            break;

        case ID_LAB2:
            OpenLab2Window(GetModuleHandle(NULL));
            break;

        case ID_LAB3:
            OpenLab3Window(GetModuleHandle(NULL));
            break;

        case ID_LAB4:
            OpenLab4Window(GetModuleHandle(NULL));
            break;

            // ДОДАНО: Виклик функції відкриття вікна ЛР №5
        case ID_LAB5:
            OpenLab5Window(GetModuleHandle(NULL));
            break;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}