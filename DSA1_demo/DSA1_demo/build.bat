@echo off
REM ============================================================================
REM Build Script for Word Frequency Analyzer
REM Compiles DSA1_demo.cpp using Visual Studio (MSVC) compiler
REM ============================================================================

setlocal enabledelayedexpansion

echo.
echo ========================================
echo   Word Frequency Analyzer - Build
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

REM Try to find Visual Studio 2022 first, then 2019
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" (
    set "VCVARSALL=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
    set VS_VERSION=2022 Community
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" (
    set "VCVARSALL=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat"
    set VS_VERSION=2022 Professional
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" (
    set "VCVARSALL=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat"
    set VS_VERSION=2022 Enterprise
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" (
    set "VCVARSALL=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
    set VS_VERSION=2019 Community
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvarsall.bat" (
    set "VCVARSALL=C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvarsall.bat"
    set VS_VERSION=2019 Professional
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" (
    set "VCVARSALL=C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat"
    set VS_VERSION=2019 Enterprise
) else (
    echo [ERROR] Visual Studio not found!
    echo.
    echo Please install Visual Studio 2019 or 2022 with C++ build tools.
    echo Download from: https://visualstudio.microsoft.com/downloads/
    pause
    exit /b 1
)

echo [SUCCESS] Found Visual Studio %VS_VERSION%
echo.

REM Setup Visual Studio environment
echo [2/5] Setting up build environment...
call "%VCVARSALL%" x64 >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Failed to setup Visual Studio environment!
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
