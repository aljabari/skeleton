@echo off
rem Locates the Emscripten SDK (via emcmake.bat on PATH) and points
rem EMSDK_PYTHON at the Python the emsdk ships. The emsdk's own emsdk_env.bat
rem only prints the environment instead of setting it, and Emscripten 5.x needs
rem Python 3.10+, so this ensures emcc/emcmake/emrun run under a compatible
rem interpreter. Called from build_emscripten.bat and run_emscripten.bat.

for /f "delims=" %%I in ('where emcmake.bat 2^>nul') do set "EMSDK_EMSCRIPTEN_DIR=%%~dpI"
if defined EMSDK_EMSCRIPTEN_DIR if exist "%EMSDK_EMSCRIPTEN_DIR%..\..\python\3.13.3_64bit\python.exe" (
    set "EMSDK_PYTHON=%EMSDK_EMSCRIPTEN_DIR%..\..\python\3.13.3_64bit\python.exe"
)
