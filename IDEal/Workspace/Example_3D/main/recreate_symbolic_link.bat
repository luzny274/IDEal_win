REM This script has to be run as administrator to work properly
set cur=%0\..
if exist "%cur%\Bibliotekum_Ultimatum" rmdir /S /Q %cur%\Bibliotekum_Ultimatum
mklink /D %cur%\Bibliotekum_Ultimatum\ %cur%\..\..\..\Link\Bibliotekum_Ultimatum\
pause
