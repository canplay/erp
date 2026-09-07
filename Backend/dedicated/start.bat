@echo off
setlocal enabledelayedexpansion

@echo off
set "SCRIPT_DIR=%~dp0"

start "" "%SCRIPT_DIR%moon\moon.exe" "main.lua"