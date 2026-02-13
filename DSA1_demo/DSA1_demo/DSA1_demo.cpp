#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <chrono>
#include <iomanip>

#pragma comment(lib, "comctl32.lib")

// Global variables
HINSTANCE hInst;
HWND hwndMain;
HWND hwndFileDisplay;
HWND hwndSearchBox;
HWND hwndSearchButton;
HWND hwndResultList;
HWND hwndLoadButton;
HWND hwndStatusBar;
HWND hwndStatsDisplay;
HWND hwndProgressBar;

std::unordered_map<std::string, int> wordFrequencyMap;
std::string currentFileName;
int totalWords = 0;

// File size limit to prevent crashes (adjustable)
const size_t MAX_FILE_SIZE = 10 * 1024 * 1024; // 10 MB default

// Function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void LoadTextFile(HWND hwnd);
void ProcessTextFile(const std::string& filepath);
void SearchWords(HWND hwnd);
std::string ToLower(const std::string& str);
void UpdateStats();
std::wstring StringToWString(const std::string& str);
size_t GetFileSize(const std::string& filepath);

// Main entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;

    // Initialize common controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LISTVIEW_CLASSES | ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);

    // Register window class
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"WordFrequencyAnalyzer";

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, L"Window Registration Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Create main window
    hwndMain = CreateWindowEx(
        0,
        L"WordFrequencyAnalyzer",
        L"Unordered Map - Word Frequency Analyzer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 900, 700,
        NULL, NULL, hInstance, NULL
    );

    if (hwndMain == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwndMain, nCmdShow);
    UpdateWindow(hwndMain);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

// Convert std::string to std::wstring for Unicode
std::wstring StringToWString(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

// Get file size in bytes
size_t GetFileSize(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) return 0;
    return static_cast<size_t>(file.tellg());
}

// Window procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        // Load File Button
        hwndLoadButton = CreateWindow(
            L"BUTTON", L"Load Text File",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            20, 20, 150, 40,
            hwnd, (HMENU)1, hInst, NULL
        );

        // File Display Label
        CreateWindow(
            L"STATIC", L"No file loaded",
            WS_VISIBLE | WS_CHILD | SS_LEFT,
            190, 20, 680, 20,
            hwnd, (HMENU)2, hInst, NULL
        );
        hwndFileDisplay = GetDlgItem(hwnd, 2);

        // Stats Display
        hwndStatsDisplay = CreateWindow(
            L"STATIC", L"Total unique words: 0 | Total words: 0 | Load time: 0ms",
            WS_VISIBLE | WS_CHILD | SS_LEFT,
            190, 45, 680, 20,
            hwnd, (HMENU)3, hInst, NULL
        );

        // Progress Bar (initially hidden)
        hwndProgressBar = CreateWindowEx(
            0, PROGRESS_CLASS, NULL,
            WS_CHILD | PBS_SMOOTH,
            190, 70, 680, 20,
            hwnd, (HMENU)8, hInst, NULL
        );
        SendMessage(hwndProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
        SendMessage(hwndProgressBar, PBM_SETSTEP, (WPARAM)1, 0);

        // Search Label
        CreateWindow(
            L"STATIC", L"Search Keywords (comma-separated):",
            WS_VISIBLE | WS_CHILD | SS_LEFT,
            20, 100, 250, 20,
            hwnd, NULL, hInst, NULL
        );

        // Search Input Box
        hwndSearchBox = CreateWindowEx(
            WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
            20, 125, 600, 25,
            hwnd, (HMENU)4, hInst, NULL
        );

        // Search Button
        hwndSearchButton = CreateWindow(
            L"BUTTON", L"Search",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            640, 125, 100, 25,
            hwnd, (HMENU)5, hInst, NULL
        );

        // Results Label
        CreateWindow(
            L"STATIC", L"Search Results:",
            WS_VISIBLE | WS_CHILD | SS_LEFT,
            20, 165, 150, 20,
            hwnd, NULL, hInst, NULL
        );

        // Result List (ListView)
        hwndResultList = CreateWindowEx(
            0, WC_LISTVIEW, L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SINGLESEL,
            20, 190, 840, 400,
            hwnd, (HMENU)6, hInst, NULL
        );

        // Set up ListView columns
        LVCOLUMN lvc;
        lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

        lvc.iSubItem = 0;
        lvc.pszText = (LPWSTR)L"Keyword";
        lvc.cx = 300;
        ListView_InsertColumn(hwndResultList, 0, &lvc);

        lvc.iSubItem = 1;
        lvc.pszText = (LPWSTR)L"Frequency";
        lvc.cx = 150;
        ListView_InsertColumn(hwndResultList, 1, &lvc);

        lvc.iSubItem = 2;
        lvc.pszText = (LPWSTR)L"Lookup Time (nanosecond)";
        lvc.cx = 180;
        ListView_InsertColumn(hwndResultList, 2, &lvc);

        lvc.iSubItem = 3;
        lvc.pszText = (LPWSTR)L"Status";
        lvc.cx = 180;
        ListView_InsertColumn(hwndResultList, 3, &lvc);

        // Status Bar
        hwndStatusBar = CreateWindowEx(
            0, STATUSCLASSNAME, L"",
            WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
            0, 0, 0, 0,
            hwnd, (HMENU)7, hInst, NULL
        );

        SetWindowText(hwndStatusBar, L"Ready. Load a text file to begin.");

        break;
    }

    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
        case 1: // Load File Button
            LoadTextFile(hwnd);
            break;

        case 5: // Search Button
            SearchWords(hwnd);
            break;
        }
        break;
    }

    case WM_SIZE: {
        // Resize status bar
        SendMessage(hwndStatusBar, WM_SIZE, 0, 0);
        break;
    }

    case WM_DESTROY: {
        PostQuitMessage(0);
        break;
    }

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// Load text file dialog
void LoadTextFile(HWND hwnd) {
    OPENFILENAME ofn;
    wchar_t szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
    ofn.lpstrFilter = L"Text Files\0*.TXT;*.LOG;*.MD\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE) {
        // Convert wide string to narrow string
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, szFile, -1, NULL, 0, NULL, NULL);
        std::string strFilename(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, szFile, -1, &strFilename[0], size_needed, NULL, NULL);
        strFilename.resize(size_needed - 1); // Remove null terminator

        currentFileName = strFilename;
        ProcessTextFile(currentFileName);
    }
}

// Convert string to lowercase
std::string ToLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Process the text file and build unordered_map
void ProcessTextFile(const std::string& filepath) {
    // Check file size first
    size_t fileSize = GetFileSize(filepath);

    // Convert bytes to MB for display
    double fileSizeMB = fileSize / (1024.0 * 1024.0);

    if (fileSize > MAX_FILE_SIZE) {
        std::wostringstream msg;
        msg << L"File size (" << std::fixed << std::setprecision(2) << fileSizeMB
            << L" MB) exceeds the " << (MAX_FILE_SIZE / (1024 * 1024))
            << L" MB limit.\n\nProcessing very large files may cause the application to freeze or crash."
            << L"\n\nDo you want to continue anyway?";

        int result = MessageBox(hwndMain, msg.str().c_str(), L"Large File Warning",
            MB_YESNO | MB_ICONWARNING);

        if (result != IDYES) {
            SetWindowText(hwndStatusBar, L"File loading cancelled.");
            return;
        }
    }

    // Show progress bar and update status
    ShowWindow(hwndProgressBar, SW_SHOW);
    SendMessage(hwndProgressBar, PBM_SETPOS, 0, 0);
    SetWindowText(hwndStatusBar, L"Processing file... Please wait...");
    UpdateWindow(hwndMain);

    auto startTime = std::chrono::high_resolution_clock::now();

    // Clear previous data
    wordFrequencyMap.clear();
    totalWords = 0;
    ListView_DeleteAllItems(hwndResultList);

    // Reserve space to reduce reallocations
    wordFrequencyMap.reserve(10000); // Pre-allocate for better performance

    // Open file in binary mode to handle different line endings properly
    std::ifstream file(filepath, std::ios::in);
    if (!file.is_open()) {
        ShowWindow(hwndProgressBar, SW_HIDE);
        MessageBox(hwndMain, L"Failed to open file!", L"Error", MB_ICONERROR | MB_OK);
        return;
    }

    // Disable sync with stdio for better performance
    file.sync_with_stdio(false);

    std::string word;
    int wordsProcessed = 0;
    int progressUpdateInterval = 1000; // Update progress every 1000 words

    while (file >> word) {
        // Remove punctuation and convert to lowercase
        std::string cleanWord;
        for (char c : word) {
            // Only process ASCII alphanumeric characters to avoid UTF-8 issues
            if (std::isalnum(static_cast<unsigned char>(c))) {
                cleanWord += c;
            }
        }

        if (!cleanWord.empty()) {
            std::string lowerWord = ToLower(cleanWord);
            wordFrequencyMap[lowerWord]++;
            totalWords++;
            wordsProcessed++;

            // Update progress bar periodically
            if (wordsProcessed % progressUpdateInterval == 0) {
                // Estimate progress based on words processed vs estimated total
                // Using word count estimation instead of tellg() which can fail with CRLF
                int estimatedTotalWords = (fileSize > 0) ? (int)(fileSize / 5) : 100000; // Assume avg 5 bytes per word
                int progress = (estimatedTotalWords > 0) ? (int)((wordsProcessed * 100) / estimatedTotalWords) : 0;
                if (progress > 100) progress = 100; // Cap at 100%
                SendMessage(hwndProgressBar, PBM_SETPOS, progress, 0);

                // Allow UI to update
                MSG msg;
                while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
        }
    }

    file.close();

    // Complete the progress bar
    SendMessage(hwndProgressBar, PBM_SETPOS, 100, 0);

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    // Hide progress bar after completion
    ShowWindow(hwndProgressBar, SW_HIDE);

    // Update UI
    std::string filename = filepath.substr(filepath.find_last_of("\\/") + 1);
    std::string displayText = "Loaded: " + filename;
    SetWindowText(hwndFileDisplay, StringToWString(displayText).c_str());

    // Update stats
    std::ostringstream stats;
    stats << "Total unique words: " << wordFrequencyMap.size()
        << " | Total words: " << totalWords
        << " | File size: " << std::fixed << std::setprecision(2) << fileSizeMB << " MB"
        << " | Load time: " << duration.count() << "ms";
    SetWindowText(hwndStatsDisplay, StringToWString(stats.str()).c_str());

    std::ostringstream status;
    status << "Loaded " << wordFrequencyMap.size() << " unique words from " << filename;
    SetWindowText(hwndStatusBar, StringToWString(status.str()).c_str());
}

// Search for words in the unordered_map
void SearchWords(HWND hwnd) {
    if (wordFrequencyMap.empty()) {
        MessageBox(hwnd, L"Please load a text file first!", L"No Data", MB_ICONWARNING | MB_OK);
        return;
    }

    wchar_t searchText[1024];
    GetWindowText(hwndSearchBox, searchText, sizeof(searchText) / sizeof(wchar_t));

    if (wcslen(searchText) == 0) {
        MessageBox(hwnd, L"Please enter at least one keyword!", L"Empty Search", MB_ICONWARNING | MB_OK);
        return;
    }

    // Convert wide string to narrow string
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, searchText, -1, NULL, 0, NULL, NULL);
    std::string strSearchText(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, searchText, -1, &strSearchText[0], size_needed, NULL, NULL);
    strSearchText.resize(size_needed - 1); // Remove null terminator

    // Clear previous results
    ListView_DeleteAllItems(hwndResultList);

    // Parse keywords (comma-separated)
    std::vector<std::string> keywords;
    std::stringstream ss(strSearchText);
    std::string keyword;

    while (std::getline(ss, keyword, ',')) {
        // Trim whitespace
        keyword.erase(0, keyword.find_first_not_of(" \t\n\r"));
        keyword.erase(keyword.find_last_not_of(" \t\n\r") + 1);

        if (!keyword.empty()) {
            keywords.push_back(ToLower(keyword));
        }
    }

    // Search each keyword
    int itemIndex = 0;
    for (const auto& kw : keywords) {
        auto startTime = std::chrono::high_resolution_clock::now();

        auto it = wordFrequencyMap.find(kw);

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);

        LVITEM lvi;
        ZeroMemory(&lvi, sizeof(lvi));
        lvi.mask = LVIF_TEXT;
        lvi.iItem = itemIndex;

        // Keyword column
        lvi.iSubItem = 0;
        std::wstring wKeyword = StringToWString(kw);
        lvi.pszText = const_cast<wchar_t*>(wKeyword.c_str());
        ListView_InsertItem(hwndResultList, &lvi);

        // Frequency column
        lvi.iSubItem = 1;
        if (it != wordFrequencyMap.end()) {
            std::wstring freq = std::to_wstring(it->second);
            ListView_SetItemText(hwndResultList, itemIndex, 1, const_cast<wchar_t*>(freq.c_str()));
        }
        else {
            ListView_SetItemText(hwndResultList, itemIndex, 1, (LPWSTR)L"0");
        }

        // Lookup time column
        lvi.iSubItem = 2;
        std::wstring timeString = std::to_wstring(duration.count());
        ListView_SetItemText(hwndResultList, itemIndex, 2, const_cast<wchar_t*>(timeString.c_str()));

        // Status column
        lvi.iSubItem = 3;
        if (it != wordFrequencyMap.end()) {
            ListView_SetItemText(hwndResultList, itemIndex, 3, (LPWSTR)L"Found");
        }
        else {
            ListView_SetItemText(hwndResultList, itemIndex, 3, (LPWSTR)L"Not Found");
        }

        itemIndex++;
    }

    std::ostringstream status;
    status << "Searched " << keywords.size() << " keyword(s). Lookup time precision: nanoseconds (O(1) constant time)";
    SetWindowText(hwndStatusBar, StringToWString(status.str()).c_str());
}