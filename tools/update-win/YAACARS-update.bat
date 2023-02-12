@echo off

set PATH=
set UPDATE=YAACARS-update

set LEVEL=release
if "%1" == "testing" set LEVEL=testing
cd %~dp0

echo ****************************************************************************
echo %~n0 working in %cd% ...
echo ****************************************************************************
if "%~n0" == "YAACARS-update" (
	rem make sure update.bat and 7z be updated, too
	copy 7z\7z.exe . 
	copy 7z\7z.dll . 
	copy 7z\7-zip.dll .
	copy %UPDATE%.bat %UPDATE%-Run.bat
	start ""    "%~dp0%UPDATE%-Run.bat" %LEVEL%
) else (
	echo *** DOWNLOADING YAACARS
	if exist %UPDATE%.zip del %UPDATE%.zip
	wget\wget --show-progress "http://www.ftw-sim.de/yaacars/download/update.php?os=win32&level=%LEVEL%" -o %UPDATE%.log -O %UPDATE%.zip
	rem not a good idea in a detached process IF %ERRORLEVEL% NEQ 0 pause
	echo ****************************************************************************
	echo *** UPACKING    YAACARS
	7z x -y -o..\.. %UPDATE%.zip
	rem IF %ERRORLEVEL% NEQ 0 pause
	echo ****************************************************************************
	echo The Updater has finished! Press a key to restart YAACARS ...
	echo ****************************************************************************
	echo.
	pause
	del 7z.exe 
	del 7z.dll 
	del 7-zip.dll
	echo ****************************************************************************
	echo *** RESTARTING  YAACARS
	start yaacars.exe
	echo ****************************************************************************
	exit
)
