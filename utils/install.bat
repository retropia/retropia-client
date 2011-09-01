set UTILSDIR=%~dps0
set SOURCE=%~fs1
set TARGET=%~fs2
for /D %%D IN ("%SOURCE%\*") DO (
	pushd 
	for /F "delims=" %%F IN ('"%UTILSDIR%\find.exe" -type f') DO (
		if exist "%TARGET%\%%F" (
			move /Y "%TARGET%\%%F" "%TARGET%\%%F.retropia-old"
			attrib +H "%TARGET%\%%F.retropia-old"
		)
	)

	xcopy /Y /E /I /R /C "%%~fsD\*" "%TARGET%"
	popd
)

exit /B 0
