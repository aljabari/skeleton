@echo off

setlocal

set REPO_ROOT=%~dp0..\..
set BUILD_DIR=%REPO_ROOT%\build\html5
set RUNTIME_DIR=%BUILD_DIR%\skeleton
set PAGE=skeleton.html

call "%~dp0build_emscripten.bat" %* || exit /b 1

if not exist "%RUNTIME_DIR%\%PAGE%" (
    echo Build output not found: %RUNTIME_DIR%\%PAGE%
    exit /b 1
)

where emrun >nul 2>&1
if errorlevel 1 (
    echo emrun not found on PATH. Activate the Emscripten SDK first, e.g.:
    echo     emsdk activate latest
    echo     emsdk_env.bat
    exit /b 1
)

rem Point EMSDK_PYTHON at the emsdk's Python so emrun runs correctly (see
rem emsdk_env.bat). build_emscripten.bat's own environment is local to it.
call "%~dp0emsdk_env.bat"

echo.
echo Serving %RUNTIME_DIR% and opening %PAGE% in the default browser...
echo Press Ctrl+C to stop the server.
emrun --serve_root "%RUNTIME_DIR%" "%RUNTIME_DIR%\%PAGE%"
