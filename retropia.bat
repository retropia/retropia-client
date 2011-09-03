@echo off
%~d0
cd "%~dp0"

@del /S /A:H *.retropia-old > nul 2>&1

echo Checking for updates... (this may take a short while)
echo.
set VERSION_FILE=version.txt
set LOCAL_VERSION="0.0.0"
if exist %VERSION_FILE% set /p LOCAL_VERSION= < %VERSION_FILE%

set REMOTE_VERSION=
set VERSION_CHECK=
@for /f "delims=" %%a in ('utils\curl.exe -s --cacert etc\cacert.pem https://raw.github.com/definitelylion/retropia-client/stable/version.txt') do @set REMOTE_VERSION=%%a

@for /f "delims=" %%a in ('utils\compare_versions.exe %LOCAL_VERSION% %REMOTE_VERSION%') do @set VERSION_CHECK=%%a

if not "%VERSION_CHECK%" == "<" goto begin

set /p DOWNLOAD_NEW_VERSION="A new version (v%REMOTE_VERSION%) is available! Press ENTER to download and install..."
echo.
echo Downloading... this may take a while.
echo.
set DOWNLOAD_LOCATION="%TEMP%\retropia-client-v%REMOTE_VERSION%.zip"
set DOWNLOAD_UNCOMPRESSED="%TEMP%\retropia-client-v%REMOTE_VERSION%"
@for /f "delims=" %%I in ('echo %DOWNLOAD_UNCOMPRESSED%') do SET DOWNLOAD_UNCOMPRESSED=%%~fsI
@del /Q %DOWNLOAD_LOCATION% > nul 2>&1
"utils\curl.exe" -L --cacert "etc\cacert.pem" -o %DOWNLOAD_LOCATION% "https://github.com/definitelylion/retropia-client/zipball/stable"
echo.
if %ERRORLEVEL% NEQ 0 (
	echo Encountered an error while downloading update. Try again later.
	goto end
)
echo Download complete.
echo.
@rmdir /S /Q %DOWNLOAD_UNCOMPRESSED% > nul 2>&1
"utils\7za" x -y -o%DOWNLOAD_UNCOMPRESSED% %DOWNLOAD_LOCATION% > nul
echo If you are installing to a system location, you may now be presented 
echo with a User Account Control (UAC) window. If so, please click "Yes" 
echo or "Allow".
@cscript //nologo "utils\install.vbs" "%~dps0\utils\install.bat" %DOWNLOAD_UNCOMPRESSED% "%~dps0" > NUL 2>&1
echo.
echo Update complete. Please restart the client.
goto end

:begin
set CONFIGDIR=%APPDATA%\retropia
set CONFIGFILE=%CONFIGDIR%\config.ini
set NICK=
set REGION=

REM default game settings

set S_DEF_NLOCALPLAYERS=1
set S_DEF_PROTECT_GAME=n
set S_DEF_EXTRA_PARAMS=

set S_NLOCALPLAYERS=%S_DEF_NLOCALPLAYERS%
set S_PROTEECT_GAME=%S_DEF_PROTEECT_GAME%
set S_EXTRA_PARAMS=%S_DEF_EXTRA_PARAMS%

REM end default game settings

REM begin configuration

if not exist "%CONFIGFILE%" (
  mkdir "%CONFIGDIR%"
  type nul > "%CONFIGFILE%"
)

for /F "usebackq tokens=1,2 delims==" %%a in ("%CONFIGFILE%") do ( 
if %%a==nick set NICK=%%b
if %%a==region set REGION=%%b
)

if not "%NICK%" == "" if not "%REGION%" == "" goto prelaunch

cls
echo CONFIGURATION (FIRST TIME ONLY)
echo ===============================
set /p NICK="Nickname: "

:region
set VALID_REGION=false
set /p REGION="Region (eu/na-west/na-east): "
if "%REGION%" == "eu" set VALID_REGION=true
if "%REGION%" == "na-west" set VALID_REGION=true
if "%REGION%" == "na-east" set VALID_REGION=true
if "%VALID_REGION%" == "false" goto region

echo nick=%NICK%> "%CONFIGFILE%"
echo region=%REGION%>> "%CONFIGFILE%"

REM end configuration

:prelaunch
set GAMEFILE="%~1"
if %GAMEFILE% == "" for /f "delims=" %%F IN ('utils\game_browser.exe') DO SET GAMEFILE="%%F"
if %GAMEFILE% == "" goto usage

cls
@del /Q "emulators\mednafen\stdout.txt" > nul 2>&1
"emulators\mednafen\mednafen.exe" -stat %GAMEFILE%
type "emulators\mednafen\stdout.txt"
echo.

:set_gamekey
set GAMEKEY=
set /p GAMEKEY="Room name that you want to create or join (max 12 chars.): "
set errorlevel=
"utils\check_key.exe" "%GAMEKEY%"
if errorlevel 1 goto set_gamekey

set CHANGESETTINGS=
set /p CHANGESETTINGS="Change default game settings before launching? (N)o/(y)es/(q)uit: "
if "%CHANGESETTINGS%" == "y" goto def_gamesettings
if "%CHANGESETTINGS%" == "q" goto end

:launch
set NETPROTECT=0
if "%S_PROTECT_GAME%" == "y" set NETPROTECT=1
"emulators\mednafen\mednafen.exe" -connect -nethost %REGION%.retropia.org -netport 4046 -netnick "%NICK%" -netprotect %NETPROTECT% -netlocalplayers %S_NLOCALPLAYERS% -netgamekey "%GAMEKEY%" %GAMEFILE%
goto end

:def_gamesettings
set S_NLOCALPLAYERS=%S_DEF_NLOCALPLAYERS%
set S_PROTECT_GAME=%S_DEF_PROTECT_GAME%
set S_EXTRA_PARAMS=%S_DEF_EXTRA_PARAMS%

:gamesettings
set GAMESETTINGCHOICE=
cls
echo GAME SETTINGS
echo =============
echo 1. Number of local players = %S_NLOCALPLAYERS% (default: %S_DEF_NLOCALPLAYERS%)
echo 2. Make game protected = %S_PROTECT_GAME% (default: %S_DEF_PROTECT_GAME%)
echo 3. Additional parameters = "%S_EXTRA_PARAMS%" (default: "%S_DEF_EXTRA_PARAMS%")
echo. 
echo Select an option [1-3] and press ENTER. Press 'c' and ENTER continue, 
echo or 'd' and ENTER to restore the default settings.
set /p GAMESETTINGCHOICE="Option: "

if "%GAMESETTINGCHOICE%" == "c" goto launch
if "%GAMESETTINGCHOICE%" == "d" goto def_gamesettings
if "%GAMESETTINGCHOICE%" == "1" goto gamesetting_1
if "%GAMESETTINGCHOICE%" == "2" goto gamesetting_2
if "%GAMESETTINGCHOICE%" == "3" goto gamesetting_3
goto gamesettings

:gamesetting_1
cls
echo Number of local players
echo =======================
echo.
echo Change this setting to match the number of people that will be 
echo playing on your computer. For example, if you and a friend are 
echo playing against others on retropia, set this value to 2.
echo.
echo Current value: %S_NLOCALPLAYERS%
set /p S_NLOCALPLAYERS="New value: "
goto gamesettings

:gamesetting_2
cls
echo Make game protected
echo ===================
echo.
echo Protected game rooms will not have their name visible in the
echo lobby. Thus, only people who know the name will be able to
echo connect. In this case, treat the name like a password and
echo choose a value that's hard to guess!
echo.
echo Current value: %S_PROTECT_GAME%
set /p S_PROTECT_GAME_TMP="New value (y/n): "
if not "%S_PROTECT_GAME_TMP%" == "y" if not "%S_PROTECT_GAME_TMP%" == "n" goto gamesetting_2
set S_PROTECT_GAME=%S_PROTECT_GAME_TMP%
goto gamesettings

:gamesetting_3
cls
echo Additional parameters
echo =====================
echo.
echo For advanced users only! If you want to send additional parameters
echo to the emulator, add them here. 
echo.
echo WARNING: some parameters can cause stability problems! Be careful!
echo.
echo HINT: Consult http://retropia.org/docs/mednafen for 
echo more information about available parameters.
echo.
echo Current value: "%S_EXTRA_PARAMS%"
set /p S_EXTRA_PARAMS="New value (no quotes): "
goto gamesettings

:usage
echo You must supply a game file. Please do so by either:
echo   - selecting the file in the "Open file" dialog  
echo   - dragging and dropping the game file onto the retropia application 
echo   - if running from a command prompt, pass the path to the game file
echo     as a parameter

:end
echo.
pause