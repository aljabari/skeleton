@echo off

setlocal

set REPO_ROOT=%~dp0..\..
set BUILD_DIR=%REPO_ROOT%\build\html5
set BUILD_TYPE=Release

if not "%~1"=="" set BUILD_TYPE=%~1

where emcmake >nul 2>&1
if errorlevel 1 (
    echo emcmake not found on PATH. Activate the Emscripten SDK first, e.g.:
    echo     emsdk activate latest
    echo     emsdk_env.bat
    exit /b 1
)

rem Point EMSDK_PYTHON at the emsdk's Python (see emsdk_env.bat).
call "%~dp0emsdk_env.bat"

where ninja >nul 2>&1
if errorlevel 1 (
    echo ninja not found on PATH. Install it via the Emscripten SDK:
    echo     emsdk install ninja
    echo     emsdk activate ninja
    exit /b 1
)

pushd "%REPO_ROOT%"

emcmake cmake -B build\html5 -G Ninja -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DSKELETON_BUILD_TESTS=OFF || exit /b 1
cmake --build build\html5 --target skeleton || exit /b 1

popd

echo.
echo Build complete. The runtime is at %BUILD_DIR%\skeleton\skeleton.html ^(plus skeleton.js and skeleton.wasm^).
