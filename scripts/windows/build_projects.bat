@echo off

setlocal

set REPO_ROOT=%~dp0..\..
set BUILD_DIR=%REPO_ROOT%\build\windows

if not exist "%BUILD_DIR%" (
    pushd "%REPO_ROOT%"
    cmake -B build\windows -G "Visual Studio 18" || exit /b 1
    popd
)

start "" "%BUILD_DIR%\skeleton.slnx"
