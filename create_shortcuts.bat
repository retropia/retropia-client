@echo off

utils\mkshortcut desktop retropia "%~dp0\retropia.exe"
utils\mkshortcut startmenu retropia\retropia "%~dp0\retropia.exe"
utils\mkshortcut startmenu "retropia\edit configuration" "%~dp0\edit_configuration.bat"