@echo off
setlocal

watchexec -w build\main.asm -e c -- "type build\main.asm"
