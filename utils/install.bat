@echo off
set UTILSDIR="%~dp0"
set SOURCE="%~1"
set TARGET="%~2"
for /D %%D IN ("%SOURCE%\*") DO (
	pushd 
	for /F "delims=" %%F IN ('"%UTILSDIR%\find.exe" -type f') DO (
		if exist "%TARGET%\%%F" (
			move /Y "%TARGET%\%%F" "%TARGET%\%%F.retropia-old"
			attrib +H "%TARGET%\%%F.retropia-old"
		)
	)

	xcopy /Y /E /I /R /C "%%D\*" "%TARGET%"
	popd
)

exit /B 0
