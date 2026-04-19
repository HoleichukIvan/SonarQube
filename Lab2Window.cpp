#include <windows.h>
#include <commdlg.h>
#include <string>
#include <fstream>
#include "Lab2Window.h"
#include "MD5.h"

using namespace std;

#define ID_MD5_RUN 500
#define ID_VERIFY   503
#define ID_INPUT   501
#define ID_OUTPUT  502
#define ID_FILE_OPEN 504

HWND hInput2, hOutput2;

LRESULT CALLBACK Lab2Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//Ui for file
void HashSelectedFile(HWND hwnd) {
    OPENFILENAMEW ofn;
    wchar_t szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"All Files\0*.*\0Text Files\0*.txt\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameW(&ofn) == TRUE) {
        wstring hash = md5_file(szFile);

        wstring result = L"Selected File:\r\n" + wstring(szFile) +
            L"\r\n\r\nMD5 hash:\r\n" + hash;
        SetWindowText(hOutput2, result.c_str());

        wofstream file(L"lab2_results.txt");
        file << L"File:\n" << szFile << L"\n\nMD5:\n" << hash;
        file.close();
    }
}

void OpenLab2Window(HINSTANCE hInstance)
{
    const wchar_t CLASS_NAME[] = L"Lab2Window";

    WNDCLASS wc = {};
    wc.lpfnWndProc = Lab2Proc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    CreateWindowEx(
        0,
        CLASS_NAME,
        L"Лабораторна робота №2 - MD5",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 700, 450,
        NULL,
        NULL,
        hInstance,
        NULL
    );
}

void RunLab2()
{
    wchar_t buffer[2048];
    GetWindowText(hInput2, buffer, 2048);

    wstring input = buffer;

    if (input.empty())
    {
        MessageBox(NULL, L"Enter text!", L"Error", MB_OK);
        return;
    }

    wstring hash = md5(input);

    wstring result = L"Input:\r\n" + input +
        L"\r\n\r\nMD5 hash:\r\n" + hash;

    SetWindowText(hOutput2, result.c_str());

    wofstream file(L"lab2_results.txt");
    file << L"Input:\n" << input << L"\n\nMD5:\n" << hash;
    file.close();
}

void VerifyHash()
{
    wchar_t buffer[2048];
    GetWindowText(hInput2, buffer, 2048);
    wstring input = buffer;

    if (input.empty())
    {
        MessageBox(NULL, L"Enter text!", L"Error", MB_OK);
        return;
    }

    wstring currentHash = md5(input);

    wifstream file(L"lab2_results.txt");
    wstring line, savedHash;
    while (getline(file, line))
    {
        if (line.find(L"MD5:") != wstring::npos)
        {
            getline(file, savedHash);
            break;
        }
    }
    file.close();

    wstring result;
    if (currentHash == savedHash)
        result = L"Integrity OK";
    else
        result = L"Data modified";

    SetWindowText(hOutput2, result.c_str());
}

LRESULT CALLBACK Lab2Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:

        CreateWindow(L"STATIC", L"Enter text:",
            WS_VISIBLE | WS_CHILD,
            20, 20, 200, 20,
            hwnd, NULL, NULL, NULL);

        hInput2 = CreateWindow(L"EDIT", L"",
            WS_VISIBLE | WS_CHILD | WS_BORDER,
            20, 50, 640, 30,
            hwnd, (HMENU)ID_INPUT, NULL, NULL);

        CreateWindow(L"BUTTON", L"Calculate MD5",
            WS_VISIBLE | WS_CHILD,
            20, 100, 200, 40,
            hwnd, (HMENU)ID_MD5_RUN, NULL, NULL);

        CreateWindow(L"BUTTON", L"Verify Integrity",
            WS_VISIBLE | WS_CHILD,
            240, 100, 200, 40,
            hwnd, (HMENU)ID_VERIFY, NULL, NULL);

        CreateWindow(L"BUTTON", L"Open File & Hash",
            WS_VISIBLE | WS_CHILD,
            460, 100, 200, 40,
            hwnd, (HMENU)ID_FILE_OPEN, NULL, NULL);

        hOutput2 = CreateWindow(L"EDIT", L"",
            WS_VISIBLE | WS_CHILD | WS_BORDER |
            ES_MULTILINE | WS_VSCROLL,
            20, 160, 640, 200,
            hwnd, (HMENU)ID_OUTPUT, NULL, NULL);

        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_MD5_RUN:
            RunLab2();
            break;
        case ID_VERIFY:
            VerifyHash();
            break;
        case ID_FILE_OPEN:
            HashSelectedFile(hwnd);
            break;
        }
        break;

    case WM_DESTROY:
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}