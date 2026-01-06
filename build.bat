@echo off
setlocal

set FILES=src\*.c src\ecs\*.c src\utils\*.c
set LIBS=user32.lib gdi32.lib kernel32.lib
set OUT_DIR=build
set EXE_NAME=game.exe

if not exist %OUT_DIR% mkdir %OUT_DIR%

cl /TC /Zi /W4 /Od /std:c11 /nologo %FILES% /Fo%OUT_DIR%\ /Fa%OUT_DIR%\ /Fe:%OUT_DIR%\%EXE_NAME% /Fd%OUT_DIR%\compiler.pdb /link %LIBS% /SUBSYSTEM:WINDOWS %OUT_DIR%\*.obj /PDB:%OUT_DIR%\debug.pdb  

if %ERRORLEVEL% equ 0 (
	echo [OK] Build completata.	
) else (
	echo [ERR] Errore di compilazione.
)
