# Word Frequency Analyzer - Build Instructions

## Overview
This package contains batch scripts to build and run the Word Frequency Analyzer Windows application

## Files Included
1. **build.bat** - Main build script that compiles the application
2. **run.bat** - Simple launcher to run the compiled application
3. **cleanup.bat** - Removes all build artifacts and temporary files
4. **DSA1_demo.cpp** - Source code (main application file)

## Quick Start

### Building the Application
1. Double-click **build.bat** or run it from command prompt
2. The script will:
   - Clean previous build artifacts
   - Compile the source code with optimizations
   - Create DSA1_demo.exe
   - Optionally launch the application

### Running the Application
After building, you can run the application by:
- Double-clicking **run.bat**, or
- Double-clicking **DSA1_demo.exe** directly

### Cleaning Build Artifacts
To remove all compiled files and start fresh:
- Double-click **cleanup.bat**
- This removes: .exe, .obj, .pdb, .ilk files and build directories