// DSA1_demo.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "DSA1_demo.h"
#include "LRUCache.h"
#include <string>
#include <sstream>

// Global cache instance
LRUCache* g_cache = nullptr;

// Control handles
HWND hKeyInput, hValueInput, hGetButton, hPutButton, hClearButton;
HWND hCacheDisplay, hStatsDisplay;

HWND hHashRadio, hTreeRadio, hVectorRadio;
HWND hBackendLabel, hPerfLabel;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DSA1DEMO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DSA1DEMO));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DSA1DEMO));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DSA1DEMO);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    HWND hWnd = CreateWindowW(szWindowClass, L"LRU Cache Visualizer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 800, 600, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    // Initialize cache with capacity 5
    g_cache = new LRUCache(5);

    // Create GUI controls
    // Labels
    CreateWindowW(L"STATIC", L"Key (Page ID):", WS_VISIBLE | WS_CHILD,
        20, 20, 120, 20, hWnd, nullptr, hInstance, nullptr);

    CreateWindowW(L"STATIC", L"Value (Page Name):", WS_VISIBLE | WS_CHILD,
        20, 50, 140, 20, hWnd, nullptr, hInstance, nullptr);

    // Input fields
    hKeyInput = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
        160, 20, 100, 20, hWnd, nullptr, hInstance, nullptr);

    hValueInput = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER,
        160, 50, 200, 20, hWnd, nullptr, hInstance, nullptr);

    // Buttons
    hPutButton = CreateWindowW(L"BUTTON", L"PUT (Add to Cache)", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        20, 90, 150, 30, hWnd, (HMENU)1001, hInstance, nullptr);

    hGetButton = CreateWindowW(L"BUTTON", L"GET (Lookup)", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        180, 90, 150, 30, hWnd, (HMENU)1002, hInstance, nullptr);

    hClearButton = CreateWindowW(L"BUTTON", L"Clear Cache", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        340, 90, 120, 30, hWnd, (HMENU)1003, hInstance, nullptr);

    // Display areas
    CreateWindowW(L"STATIC", L"Cache State (Most Recent First):", WS_VISIBLE | WS_CHILD,
        20, 140, 300, 20, hWnd, nullptr, hInstance, nullptr);

    hCacheDisplay = CreateWindowW(L"LISTBOX", L"",
        WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_NOTIFY,
        20, 165, 440, 200, hWnd, nullptr, hInstance, nullptr);

    hStatsDisplay = CreateWindowW(L"STATIC", L"Stats: Hits: 0 | Misses: 0 | Size: 0/5",
        WS_VISIBLE | WS_CHILD,
        20, 380, 440, 40, hWnd, nullptr, hInstance, nullptr);

    // Backend selection
    CreateWindowW(L"STATIC", L"Data Structure Backend:", WS_VISIBLE | WS_CHILD,
        480, 20, 200, 20, hWnd, nullptr, hInstance, nullptr);

    hHashRadio = CreateWindowW(L"BUTTON", L"Hash Table (O(1))",
        WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,
        480, 45, 180, 20, hWnd, (HMENU)2001, hInstance, nullptr);

    hTreeRadio = CreateWindowW(L"BUTTON", L"Binary Tree (O(log n))",
        WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
        480, 70, 180, 20, hWnd, (HMENU)2002, hInstance, nullptr);

    hVectorRadio = CreateWindowW(L"BUTTON", L"Vector (O(n))",
        WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
        480, 95, 180, 20, hWnd, (HMENU)2003, hInstance, nullptr);

    // Set hash table as default
    SendMessage(hHashRadio, BM_SETCHECK, BST_CHECKED, 0);

    hBackendLabel = CreateWindowW(L"STATIC", L"Current: Hash Table (unordered_map)",
        WS_VISIBLE | WS_CHILD,
        480, 125, 280, 20, hWnd, nullptr, hInstance, nullptr);

    hPerfLabel = CreateWindowW(L"STATIC", L"Avg Access Time: 0.0 μs",
        WS_VISIBLE | WS_CHILD,
        480, 150, 280, 20, hWnd, nullptr, hInstance, nullptr);

    // Add benchmark button
    CreateWindowW(L"BUTTON", L"Run Benchmark", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        480, 180, 150, 30, hWnd, (HMENU)1004, hInstance, nullptr);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}


void UpdateDisplay(HWND hWnd) {
    // Clear listbox
    SendMessage(hCacheDisplay, LB_RESETCONTENT, 0, 0);

    // Get current cache state
    auto state = g_cache->getCurrentState();

    for (const auto& item : state) {
        wstringstream wss;
        wss << L"Page " << item.first << L": " << wstring(item.second.begin(), item.second.end());
        SendMessage(hCacheDisplay, LB_ADDSTRING, 0, (LPARAM)wss.str().c_str());
    }

    // Update stats
    wstringstream statText;
    statText << L"Stats: Hits: " << g_cache->getHitCount()
        << L" | Misses: " << g_cache->getMissCount()
        << L" | Size: " << g_cache->getSize() << L"/" << g_cache->getCapacity();
    SetWindowTextW(hStatsDisplay, statText.str().c_str());

    // Update performance label
    wstringstream perfText;
    perfText << L"Avg Access Time: "
        << fixed << setprecision(2)
        << g_cache->getAverageAccessTime() << L" μs";
    SetWindowTextW(hPerfLabel, perfText.str().c_str());

}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case 1001: // PUT button
            {
                WCHAR keyBuf[100], valueBuf[256];
                GetWindowTextW(hKeyInput, keyBuf, 100);
                GetWindowTextW(hValueInput, valueBuf, 256);

                int key = _wtoi(keyBuf);
                wstring wValue(valueBuf);
                string value(wValue.begin(), wValue.end());

                if (key > 0 && !value.empty()) {
                    g_cache->put(key, value);
                    UpdateDisplay(hWnd);

                    // Clear inputs
                    SetWindowTextW(hKeyInput, L"");
                    SetWindowTextW(hValueInput, L"");
                }
                else {
                    MessageBoxW(hWnd, L"Please enter valid key and value", L"Error", MB_OK);
                }
            }
            break;

            case 1002: // GET button
            {
                WCHAR keyBuf[100];
                GetWindowTextW(hKeyInput, keyBuf, 100);
                int key = _wtoi(keyBuf);

                if (key > 0) {
                    string result = g_cache->get(key);
                    wstring wResult(result.begin(), result.end());

                    if (result == "MISS") {
                        MessageBoxW(hWnd, L"Cache MISS - Page not in cache!", L"Result", MB_OK | MB_ICONWARNING);
                    }
                    else {
                        wstring msg = L"Cache HIT!\nPage: " + wResult;
                        MessageBoxW(hWnd, msg.c_str(), L"Result", MB_OK | MB_ICONINFORMATION);
                    }

                    UpdateDisplay(hWnd);
                }
                else {
                    MessageBoxW(hWnd, L"Please enter a valid key", L"Error", MB_OK);
                }
            }
            break;

            case 1003: // Clear button
                g_cache->clear();
                UpdateDisplay(hWnd);
                break;

            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;

            case 2001: // Hash Table selected
                g_cache->setBackend(CacheBackend::HASH_TABLE);
                SetWindowTextW(hBackendLabel, L"Current: Hash Table (unordered_map)");
                break;

            case 2002: // Binary Tree selected
                g_cache->setBackend(CacheBackend::BINARY_TREE);
                SetWindowTextW(hBackendLabel, L"Current: Binary Search Tree (map)");
                break;

            case 2003: // Vector selected
                g_cache->setBackend(CacheBackend::LINEAR_SEARCH);
                SetWindowTextW(hBackendLabel, L"Current: Linear Search (vector)");
                break;

            case 1004: // Benchmark button
            {
                // Save current cache state
                CacheBackend currentBackend = g_cache->getBackend();

                // Create a fresh cache for benchmarking
                delete g_cache;
                g_cache = new LRUCache(50, currentBackend); // Larger capacity for benchmark

                // Run benchmark with current backend
                auto start = chrono::high_resolution_clock::now();

                for (int i = 0; i < 1000; i++) {
                    string value = "Page_" + to_string(i);
                    g_cache->put(i % 100, value);
                    g_cache->get(i % 50);
                }

                auto end = chrono::high_resolution_clock::now();
                auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

                // Get backend name
                string backendName = g_cache->getBackendName();
                wstring wBackendName(backendName.begin(), backendName.end());

                // Create message
                wchar_t msg[512];
                swprintf_s(msg, 512,
                    L"Benchmark Complete!\n\n"
                    L"Backend: %s\n"
                    L"Operations: 1000 PUTs + 1000 GETs\n"
                    L"Total Time: %lld ms\n"
                    L"Avg Access Time: %.2f μs",
                    wBackendName.c_str(),
                    duration.count(),
                    g_cache->getAverageAccessTime());

                MessageBoxW(hWnd, msg, L"Benchmark Results", MB_OK | MB_ICONINFORMATION);

                // Reset cache to normal size
                delete g_cache;
                g_cache = new LRUCache(5, currentBackend);
                UpdateDisplay(hWnd);
            }
            break;
            break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
    break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        delete g_cache;
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
