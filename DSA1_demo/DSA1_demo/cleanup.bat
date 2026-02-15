@echo off
REM ============================================================================
REM Cleanup Script for Word Frequency Analyzer Project
REM Removes all build artifacts and temporary files
REM ============================================================================

echo.
echo ========================================
echo   Cleanup Build Artifacts
echo ========================================
echo.

set CLEANED=0

echo Searching for build artifacts to remove...
echo.

REM Remove executable
if exist "DSA1_demo.exe" (
    echo [FOUND] DSA1_demo.exe
    del /F /Q "DSA1_demo.exe" 2>nul
    if not exist "DSA1_demo.exe" (
        echo [DELETED] DSA1_demo.exe
        set /A CLEANED+=1
    )
)

REM Remove object files
for %%F in (*.obj) do (
    echo [FOUND] %%F
    del /F /Q "%%F" 2>nul
    echo [DELETED] %%F
    set /A CLEANED+=1
)

REM Remove program database files
for %%F in (*.pdb) do (
    echo [FOUND] %%F
    del /F /Q "%%F" 2>nul
    echo [DELETED] %%F
    set /A CLEANED+=1
)

REM Remove incremental link files
for %%F in (*.ilk) do (
    echo [FOUND] %%F
    del /F /Q "%%F" 2>nul
    echo [DELETED] %%F
    set /A CLEANED+=1
)

REM Remove intermediate directories if they exist
if exist "Debug" (
    echo [FOUND] Debug\ directory
    rmdir /S /Q "Debug" 2>nul
    if not exist "Debug" (
        echo [DELETED] Debug\ directory
        set /A CLEANED+=1
    )
)

if exist "Release" (
    echo [FOUND] Release\ directory
    rmdir /S /Q "Release" 2>nul
    if not exist "Release" (
        echo [DELETED] Release\ directory
        set /A CLEANED+=1
    )
)

if exist "x64" (
    echo [FOUND] x64\ directory
    rmdir /S /Q "x64" 2>nul
    if not exist "x64" (
        echo [DELETED] x64\ directory
        set /A CLEANED+=1
    )
)

REM Remove Visual Studio user files
if exist "*.user" (
    for %%F in (*.user) do (
        echo [FOUND] %%F
        del /F /Q "%%F" 2>nul
        echo [DELETED] %%F
        set /A CLEANED+=1
    )
)

if exist ".vs" (
    echo [FOUND] .vs\ directory
    rmdir /S /Q ".vs" 2>nul
    if not exist ".vs" (
        echo [DELETED] .vs\ directory
        set /A CLEANED+=1
    )
)

echo.
echo ========================================
echo   Cleanup Complete!
echo ========================================
echo.

if %CLEANED% GTR 0 (
    echo [SUCCESS] Removed %CLEANED% item(s)
) else (
    echo [INFO] No build artifacts found - directory is already clean
)

echo.
echo Press any key to exit...
pause >nul

exit /b 0
