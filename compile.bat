@echo off
setlocal EnableDelayedExpansion

echo ============================================
echo  Indra Plugin - Build Script
echo ============================================
echo.

set BUILD_CONFIG=Release
set BUILD_DIR=build
set CMAKE_GENERATOR=Visual Studio 17 2022

:: -----------------------------------------------
:: Check for CMake
:: -----------------------------------------------
where cmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake not found. Please install CMake and add it to your PATH.
    echo         Download: https://cmake.org/download/
    pause
    exit /b 1
)
echo [OK] CMake found.

:: -----------------------------------------------
:: Check local include folder
:: -----------------------------------------------
if not exist "include\EuroScopePlugIn.h" (
    echo [ERROR] include\EuroScopePlugIn.h not found.
    echo         Make sure EuroScopePlugIn.h is in the include\ folder
    echo         next to this compile.bat script.
    pause
    exit /b 1
)
echo [OK] EuroScopePlugIn.h found.

:: -----------------------------------------------
:: Create build directory
:: -----------------------------------------------
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

:: -----------------------------------------------
:: CMake configure
:: -----------------------------------------------
echo.
echo [INFO] Configuring...
echo.

cmake -S . -B "%BUILD_DIR%" -G "%CMAKE_GENERATOR%" -A Win32

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake configuration failed.
    pause
    exit /b 1
)

:: -----------------------------------------------
:: CMake build
:: -----------------------------------------------
echo.
echo [INFO] Building (%BUILD_CONFIG%)...
echo.

cmake --build "%BUILD_DIR%" --config %BUILD_CONFIG% --parallel

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Build failed.
    pause
    exit /b 1
)

echo.
echo ============================================
echo  BUILD SUCCESSFUL
echo ============================================
echo  Output: %BUILD_DIR%\%BUILD_CONFIG%\IndraPlugin.dll
echo  Copy IndraPlugin.dll to your EuroScope Plugins folder.
echo ============================================
echo.
exit /b 0