@echo off
REM ============================================================================
REM Universal Build Script for Word Frequency Analyzer
REM Uses vswhere.exe for automatic Visual Studio detection (all versions)
REM Falls back to manual search for VS 2022-2030
REM ============================================================================

setlocal enabledelayedexpansion

echo.
echo ========================================
echo   Word Frequency Analyzer - Build
echo   (Universal VS Detector)
echo ========================================
echo.

REM Configuration
set SOURCE_FILE=DSA1_demo.cpp
set OUTPUT_EXE=DSA1_demo.exe
set BUILD_TYPE=Release
set VS_VERSION=

REM Check if source file exists
if not exist "%SOURCE_FILE%" (
    echo [ERROR] Source file "%SOURCE_FILE%" not found!
    echo Please make sure %SOURCE_FILE% is in the current directory.
    pause
    exit /b 1
)

echo [1/5] Locating Visual Studio installation...

REM Method 1: Try using vswhere.exe (official Microsoft tool, most reliable)
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if exist "%VSWHERE%" (
    echo [INFO] Using vswhere.exe for automatic detection...
    
    REM Find latest VS installation with C++ tools
    for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
        set "VS_INSTALL_PATH=%%i"
    )
    
    if defined VS_INSTALL_PATH (
        set "VCVARSALL=!VS_INSTALL_PATH!\VC\Auxiliary\Build\vcvarsall.bat"
        
        REM Get VS version info
        for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property displayName`) do (
            set "VS_VERSION=%%i"
        )
        
        if exist "!VCVARSALL!" (
            echo [SUCCESS] Found: !VS_VERSION!
            goto :found_vs
        )
    )
)

echo [INFO] vswhere.exe not available, using manual detection...

REM Method 2: Manual search for VS 2022-2030 (future-proof)
REM Search newest to oldest
for %%Y in (2030 2029 2028 2027 2026 2025 2024 2023 2022) do (
    for %%E in (Enterprise Professional Community) do (
        if exist "C:\Program Files\Microsoft Visual Studio\%%Y\%%E\VC\Auxiliary\Build\vcvarsall.bat" (
            set "VCVARSALL=C:\Program Files\Microsoft Visual Studio\%%Y\%%E\VC\Auxiliary\Build\vcvarsall.bat"
            set VS_VERSION=%%Y %%E
            echo [SUCCESS] Found Visual Studio %%Y %%E
            goto :found_vs
        )
    )
)

REM If still not found, error out
echo [ERROR] Visual Studio not found!
echo.
echo Please install Visual Studio 2022 or newer with C++ build tools.
echo Required component: Desktop development with C++
echo Download from: https://visualstudio.microsoft.com/downloads/
echo.
pause
exit /b 1

:found_vs
echo.

REM Setup Visual Studio environment
echo [2/5] Setting up build environment...
call "%VCVARSALL%" x64 >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Failed to setup Visual Studio environment!
    echo.
    echo Make sure the "Desktop development with C++" workload is installed.
    pause
    exit /b 1
)
echo [SUCCESS] Build environment ready
echo.

REM Clean previous build artifacts
echo [3/5] Cleaning previous build artifacts...
if exist "%OUTPUT_EXE%" (
    del /F /Q "%OUTPUT_EXE%" 2>nul
    echo     - Deleted old EXE
)
if exist "*.obj" (
    del /F /Q *.obj 2>nul
    echo     - Deleted object files
)
if exist "*.pdb" (
    del /F /Q *.pdb 2>nul
    echo     - Deleted debug files
)
if exist "*.ilk" (
    del /F /Q *.ilk 2>nul
    echo     - Deleted incremental link files
)
echo [SUCCESS] Cleanup complete
echo.

REM Compile the application
echo [4/5] Compiling %SOURCE_FILE%...
echo.
echo Compiler flags:
echo   /EHsc         - Enable C++ exception handling
echo   /std:c++17    - Use C++17 standard
echo   /O2           - Maximum optimization (Release)
echo   /W3           - Warning level 3
echo   /DUNICODE     - Enable Unicode character set
echo   /D_UNICODE    - Enable Unicode character set
echo   /Fe:%OUTPUT_EXE% - Output executable name
echo   /link         - Link with required libraries
echo.

cl.exe /EHsc /std:c++17 /O2 /W3 /DUNICODE /D_UNICODE /Fe:%OUTPUT_EXE% %SOURCE_FILE% ^
    /link user32.lib gdi32.lib comctl32.lib comdlg32.lib

if errorlevel 1 (
    echo.
    echo [ERROR] Compilation failed!
    echo Please check the error messages above.
    pause
    exit /b 1
)

echo.
echo [SUCCESS] Compilation successful!
echo.

REM Post-build cleanup (remove intermediate files)
echo [5/5] Post-build cleanup...
if exist "*.obj" (
    del /F /Q *.obj 2>nul
    echo     - Removed object files
)
if exist "*.ilk" (
    del /F /Q *.ilk 2>nul
    echo     - Removed incremental link files
)
echo [SUCCESS] Cleanup complete
echo.

REM Display build summary
echo ========================================
echo   BUILD SUCCESSFUL!
echo ========================================
echo.
echo Output: %OUTPUT_EXE%
echo Visual Studio: %VS_VERSION%
echo Build Type: %BUILD_TYPE%
echo.

REM Check if EXE was created
if exist "%OUTPUT_EXE%" (
    echo [INFO] Executable created successfully!
    echo        Size: 
    for %%A in ("%OUTPUT_EXE%") do echo        %%~zA bytes
    echo.
    echo Would you like to run the application now?
    choice /C YN /M "Run DSA1_demo.exe"
    if !errorlevel! equ 1 (
        echo.
        echo Launching %OUTPUT_EXE%...
        start "" "%OUTPUT_EXE%"
    )
) else (
    echo [WARNING] Executable not found after build!
)

echo.
echo Press any key to exit...
pause >nul

endlocal
exit /b 0
