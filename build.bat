@echo off
setlocal

set FILES=src\*.c exercises\*.c
set LIBS=user32.lib gdi32.lib kernel32.lib
set OUT_DIR=build
set EXE_NAME=game.exe
set ASM_FILE_NAME=main.asm
set LINKER_PDB_NAME=debug.pdb
set LINKER_PDB_PATH=%OUT_DIR%\%LINKER_PDB_NAME%
set COMPIILER_PDB_NAME=compiler_objects.pdb
set COMPILER_PDB_PATH=%OUT_DIR%\%COMPIILER_PDB_NAME%

if not exist %OUT_DIR% mkdir %OUT_DIR%

cl /TC /Zi /W4 /Od /nologo %FILES% /Fe:%OUT_DIR%\%EXE_NAME% /Fo%OUT_DIR%\ /Fa%OUT_DIR%\ /Fd%COMPILER_PDB_PATH% /link %LIBS% /SUBSYSTEM:CONSOLE /PDB:%LINKER_PDB_PATH%

echo.
if %ERRORLEVEL% equ 0 (
	echo [OK] Build completata.	
) else (
	echo [ERR] Errore di compilazione.
)
