@echo off
REM ============================================================================
REM Launcher for Word Frequency Analyzer
REM Simple script to run the compiled application
REM ============================================================================

echo.
echo ========================================
echo   Word Frequency Analyzer - Launch
echo ========================================
echo.

set EXE_NAME=DSA1_demo.exe

REM Check if executable exists
if not exist "%EXE_NAME%" (
    echo [ERROR] %EXE_NAME% not found!
    echo.
    echo Please build the application first by running build.bat
    echo.
    pause
    exit /b 1
)

echo [INFO] Launching %EXE_NAME%...
echo.

REM Run the application
start "" "%EXE_NAME%"

REM Optional: Wait for the process to start
timeout /t 1 /nobreak >nul

echo [SUCCESS] Application launched successfully!
echo.

exit /b 0
