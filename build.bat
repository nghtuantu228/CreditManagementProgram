@echo off
REM ============================================================
REM  build.bat - Bien dich chuong trinh "Quan ly tin dung"
REM  Su dung:   build.bat
REM  Ket qua:   credit.exe
REM ============================================================
setlocal

set VCVARS="C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
if not exist %VCVARS% (
    echo [LOI] Khong tim thay vcvars64.bat.
    echo        Hay chay lenh nay trong "x64 Native Tools Command Prompt" cua Visual Studio.
    pause
    exit /b 1
)

call %VCVARS% >nul
cd /d "%~dp0"

echo [1/2] Bien dich...
cl /nologo /W4 /O2 /D_CRT_SECURE_NO_WARNINGS /Fe:credit.exe credit.c

if errorlevel 1 (
    echo.
    echo [LOI] Bien dich that bai.
    pause
    exit /b 1
)

echo [2/2] Hoan tat.
echo.
echo Chay chuong trinh:  credit.exe
pause
