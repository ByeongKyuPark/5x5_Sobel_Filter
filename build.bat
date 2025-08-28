@echo off
REM Build script for Sobel Filter project
REM Author: BK Park

echo =======================================
echo Sobel Filter - Build Script
echo =======================================

if not exist build (
    echo Creating build directory...
    mkdir build
)

cd build

echo.
echo Configuring CMake...
cmake .. -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 (
    echo ERROR: CMake configuration failed!
    pause
    exit /b 1
)

echo.
echo Building project...
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo ERROR: Build failed!
    pause
    exit /b 1
)

echo.
echo Build completed successfully!
echo Executable location: build\Release\sobel_filter.exe
echo.
echo Usage: sobel_filter.exe input.raw output.raw
echo   - input.raw: 640x640 RGB raw image (1,228,800 bytes)
echo   - output.raw: Grayscale output image (409,600 bytes)
echo.
pause
