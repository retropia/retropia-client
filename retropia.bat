@echo off
%~d0
cd "%~dp0"
set VERSION_FILE=version.txt

if not exist %VERSION_FILE% (
  echo version.txt is missing. Please download a new copy of the client from 
  echo http://retropia.org
  goto end
)

set REMOTE_VERSION=
set VERSION_CHECK=	
set /p LOCAL_VERSION= < %VERSION_FILE%
@for /f "delims=" %%a in ('utils\curl.exe -s --cacert etc\cacert.pem https://raw.github.com/definitelylion/retropia-client/stable/version.txt') do @set REMOTE_VERSION=%%a

@for /f "delims=" %%a in ('utils\compare_versions.exe %LOCAL_VERSION% %REMOTE_VERSION%') do @set VERSION_CHECK=%%a

if not "%VERSION_CHECK%" == "<" goto begin

set /p DOWNLOAD_NEW_VERSION="A new version (v%REMOTE_VERSION%) is available! Press ENTER to download and install..."
echo.
echo Downloading... this may take a while.
echo.
set DOWNLOAD_LOCATION="%TEMP%\retropia-client-v%REMOTE_VERSION%.zip"
set DOWNLOAD_UNCOMPRESSED="%TEMP%\retropia-client-v%REMOTE_VERSION%"
del %DOWNLOAD_LOCATION%
"utils\curl.exe" -L --cacert "etc\cacert.pem" -o %DOWNLOAD_LOCATION% "https://github.com/definitelylion/retropia-client/zipball/stable"
echo.
echo Download complete.
echo.
rmdir /S /Q %DOWNLOAD_UNCOMPRESSED%
"utils\7za" x -y -o%DOWNLOAD_UNCOMPRESSED% %DOWNLOAD_LOCATION% > NUL
echo If you are using using Windows Vista or Windows 7, you may now be 
echo presented with a User Account Control (UAC) window. If so, please click 
echo "Yes" or "Allow".
cscript //nologo "utils\install.vbs" %DOWNLOAD_UNCOMPRESSED% "%~dp0"
echo.
echo Update complete. Please restart the client.
goto end

:begin
if "%~1" == "" goto usage

set CONFIGDIR=%APPDATA%\retropia
set CONFIGFILE=%CONFIGDIR%\config.ini
set NICK=
set REGION=

REM default game settings

set S_DEF_NLOCALPLAYERS=1
set S_DEF_HIDE_GAME=n
set S_DEF_EXTRA_PARAMS=

set S_NLOCALPLAYERS=%S_DEF_NLOCALPLAYERS%
set S_HIDE_GAME=%S_DEF_HIDE_GAME%
set S_EXTRA_PARAMS=%S_DEF_EXTRA_PARAMS%

REM end default game settings

REM begin configuration

if not exist %CONFIGFILE% (
  mkdir "%CONFIGDIR%"
  type nul > %CONFIGFILE%
)

for /F "usebackq tokens=1,2 delims==" %%a in (%CONFIGFILE%) do ( 
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
set /p REGION="Region (na/eu): "
if "%REGION%" == "na" set VALID_REGION=true
if "%REGION%" == "eu" set VALID_REGION=true
if "%VALID_REGION%" == "false" goto region

echo nick=%NICK%> %CONFIGFILE%
echo region=%REGION%>> %CONFIGFILE%

REM end configuration

:prelaunch

cls
@del "emulators\mednafen\stdout.txt"
"emulators\mednafen\mednafen.exe" -stat %1
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
set NETHIDDEN=0
if "%S_HIDE_GAME%" == "y" set NETHIDDEN=1
@echo on
"emulators\mednafen\mednafen.exe" -snes.input.port1.multitap 1 -snes.input.port2.multitap 1 -connect -nethost %REGION%.retropia.org -netport 4046 -netnick "%NICK%" -nethidden %NETHIDDEN% -netlocalplayers %S_NLOCALPLAYERS% -netgamekey "%GAMEKEY%" %1
goto end

:def_gamesettings
set S_NLOCALPLAYERS=%S_DEF_NLOCALPLAYERS%
set S_S_HIDE_GAME=%S_DEF_HIDE_GAME%
set S_EXTRA_PARAMS=%S_DEF_EXTRA_PARAMS%

:gamesettings
set GAMESETTINGCHOICE=
cls
echo GAME SETTINGS
echo =============
echo 1. Number of local players = %S_NLOCALPLAYERS% (default: %S_DEF_NLOCALPLAYERS%)
echo 2. Make game private = %S_HIDE_GAME% (default: %S_DEF_HIDE_GAME%)
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
goto :gamesettings

:gamesetting_2
cls
echo Make game private
echo =================
echo.
echo If you don't want your game visible in the retropia lobby, set 
echo this value to "y".
echo.
echo Current value: %S_HIDE_GAME%
set /p S_HIDE_GAME_TMP="New value (y/n): "
if not "%S_HIDE_GAME_TMP%" == "y" if not "%S_HIDE_GAME_TMP%" == "n" goto gamesetting_2
set S_HIDE_GAME=%S_HIDE_GAME_TMP%
goto :gamesettings

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
echo HINT: Consult http://mednafen.sourceforge.net/documentation/ for 
echo more information about available parameters.
echo.
echo Current value: "%S_EXTRA_PARAMS%"
set /p S_EXTRA_PARAMS="New value (no quotes): "
goto :gamesettings

:usage
echo usage: %0 [rom-file]

:end