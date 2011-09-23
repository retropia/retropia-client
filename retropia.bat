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
"utils\curl.exe" -# -L --cacert "etc\cacert.pem" -o %DOWNLOAD_LOCATION% "https://github.com/definitelylion/retropia-client/zipball/stable"
echo.
if %ERRORLEVEL% NEQ 0 (
	echo Encountered an error while downloading update. Try again later.
	goto pause_end
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
goto pause_end

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
if %%a==cdrom set CDROM=%%b
if %%a==dosdir set DOSDIR=%%b
)

if not "%NICK%" == "" if not "%REGION%" == "" if not "%CDROM%" == "" if not "%DOSDIR%" == "" goto prelaunch

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

:cdrom
set /p CDROM="CD-ROM drive (e.g. D:) or press ENTER : "
if "%CDROM%" == "" set CDROM=none
if not "%CDROM%" == "none" (
	for /f "delims=" %%I IN ('utils\dtype.exe %CDROM%') DO (
		if not "%%I" == "cdrom" (
			echo "%CDROM%" is not a CD-ROM drive.
			goto cdrom
		)
	)
)

:dosdir
set DOSDIR=
for /f "delims=" %%D IN ('utils\browse.exe -dir "Select a directory where DOS games will be installed (e.g. C:\DOSGames)"') DO (
	SET DOSDIR=%%~sD
)
if "%DOSDIR%" == "" goto dosdir

echo nick=%NICK%> "%CONFIGFILE%"
echo region=%REGION%>> "%CONFIGFILE%"
echo cdrom=%CDROM%>> "%CONFIGFILE%"
echo dosdir=%DOSDIR%>> "%CONFIGFILE%"

cls
REM end configuration

:prelaunch
set GAMEFILE="%~1"
set GAMEFILE_DRIVE=%~d1
set GAMEFILE_DIR=%~ps1
set GAMEFILE_NAME=%~nxs1
set GAMEFILE_SUFFIX="%~x1"
if %GAMEFILE% == "" for /f "delims=" %%F IN ('utils\browse.exe -file "Select a game file"') DO (
	SET GAMEFILE="%%F"
	SET GAMEFILE_DRIVE=%%~dF
	SET GAMEFILE_DIR=%%~psF
	SET GAMEFILE_NAME=%%~nxsF
	SET GAMEFILE_SUFFIX="%%~xF"
)
if %GAMEFILE% == "" goto usage

set IS_DOS_APP=0
"utils\file.exe" -b -m etc\msdos_magic %GAMEFILE% | findstr "DOS " >NUL 2>&1
if %ERRORLEVEL% EQU 0 set IS_DOS_APP=1
if %GAMEFILE_SUFFIX% == ".BAT" set IS_DOS_APP=1
if %GAMEFILE_SUFFIX% == ".bat" set IS_DOS_APP=1
if %GAMEFILE_SUFFIX% == ".COM" set IS_DOS_APP=1
if %GAMEFILE_SUFFIX% == ".com" set IS_DOS_APP=1

set MOUNTCD=
if not "%CDROM%" == "none" set MOUNTCD= -c "MOUNT D %CDROM%\ -t cdrom -ioctl"	
set CHDIR=-c "X:"
for /f "delims=" %%I IN ('utils\dtype.exe %GAMEFILE_DRIVE%') DO (	
	if "%%I" == "cdrom" (
		set CHDIR=-c "D:" -c "cd %GAMEFILE_DIR% "
	)
)

if %IS_DOS_APP% EQU 1 (
	echo Launching DOS game...
	emulators\dosbox\dosbox.exe -c "MOUNT C '%DOSDIR%'"%MOUNTCD% -c "MOUNT X '%GAMEFILE_DRIVE%%GAMEFILE_DIR%'" %CHDIR% -c "IPXNET CONNECT %REGION%.retropia.org" -c "%GAMEFILE_NAME%" -conf emulators\dosbox\dosbox.conf -noconsole
	goto end
)

"utils\file.exe" -b -m etc\msdos_magic %GAMEFILE% | findstr "PE32" >NUL 2>&1
if %ERRORLEVEL% EQU 0 (
	echo This game was designed to run on Windows.
	echo.
	echo Old Windows games may not run reliably ^(or at all^) on modern 
	echo Windows versions. In a near future, retropia will try to support 
	echo as many of these games as possible.
	echo.
	echo NOTE: If there exists a DOS version of the game you are trying to run, 
	echo you may using that instead.
	echo.
	goto pause_end
)

cls
@del /Q "emulators\mednafen\stdout.txt" > nul 2>&1
"emulators\mednafen\mednafen.exe" -stat %GAMEFILE%
set ERRLVLTMP=%ERRORLEVEL%
type "emulators\mednafen\stdout.txt"
if not %ERRLVLTMP% EQU 0 goto pause_end
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

:pause_end
echo.
pause

:end