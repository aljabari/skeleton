@echo off

setlocal

set REPO_ROOT=%~dp0..\..

if not exist "%REPO_ROOT%\build" (
    pushd "%REPO_ROOT%"
    cmake -B build -G "Visual Studio 18" || exit /b 1
    popd
)

start "" "%REPO_ROOT%\build\skeleton.slnx"
