@echo off
set CONFIGFILE=%APPDATA%\retropia\config.ini

if exist "%CONFIGFILE%" (
	start notepad "%CONFIGFILE%"
) else (
	echo No configuration file found at %CONFIGFILE%
	echo Please run retropia.exe an follow the configuration guide 
	echo at least once to create this file.
	echo.
	pause
)