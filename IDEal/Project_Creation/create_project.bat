@ECHO OFF
set cur=%0\..\..\Workspace\

echo Project name: 
set /p appName= 

echo Choose start: Blank Project (0), Basic Project (1), Basic 3D Project (2)
:loop
set /p typProj= 

if NOT "%typProj%" == "0" if NOT "%typProj%" == "1" if NOT "%typProj%" == "2" (
	echo Try again: 
	goto loop
)





mkdir %cur%\%appName%
mkdir %cur%\%appName%\main

copy %cur%\..\Link\Bibliotekum_Ultimatum\Binaries\SDL_win\SDL2.dll %cur%\%appName%\main\SDL2.dll

mklink /D %cur%\%appName%\main\Bibliotekum_Ultimatum\ %cur%\..\Link\Bibliotekum_Ultimatum\
mkdir %cur%\%appName%\main\%appName%_resources
mkdir %cur%\%appName%\main\Compilation_scripts

mkdir %cur%\%appName%\Compiled
mkdir %cur%\%appName%\Compiled\Windows
copy %cur%\..\Link\Bibliotekum_Ultimatum\Binaries\SDL_win\SDL2.dll %cur%\%appName%\Compiled\Windows\SDL2.dll




(
echo REM This script has to be run as administrator to work properly
echo set cur=%%0\..
echo if exist "%%cur%%\Bibliotekum_Ultimatum" rmdir /S /Q %%cur%%\Bibliotekum_Ultimatum
echo mklink /D %%cur%%\Bibliotekum_Ultimatum\ %%cur%%\..\..\..\Link\Bibliotekum_Ultimatum\
echo pause
) > %cur%\%appName%\main\recreate_symbolic_link.bat

echo clang++ ../main.cpp -o ../test.exe -DBU_SDL -DBU_WINDOWS -DBU_APP_NAME=\"%appName%\" -DBU_DESKTOP -std=gnu++17 ../Bibliotekum_Ultimatum/Binaries/SDL_win/SDL2main.lib ../Bibliotekum_Ultimatum/Binaries/SDL_win/SDL2.lib -lopengl32 -lshell32 -m32 -Xlinker /subsystem:console > %cur%\%appName%\main\Compilation_scripts\make.bat

(
echo clang++ ../main.cpp -o ../test.exe -DBU_SDL -DBU_WINDOWS -DBU_APP_NAME=\"%appName%\" -DBU_DESKTOP -std=gnu++17 ../Bibliotekum_Ultimatum/Binaries/SDL_win/SDL2main.lib ../Bibliotekum_Ultimatum/Binaries/SDL_win/SDL2.lib -lopengl32 -lshell32 -m32 -Xlinker /subsystem:console
echo cd ..
echo test.exe
) > %cur%\%appName%\main\Compilation_scripts\make-test.bat

(
echo set cur=%%0\..
echo clang++ ../main.cpp -o ../../Compiled/Windows/%appName%.exe -DBU_SDL -DBU_WINDOWS -DBU_APP_NAME=\"%appName%\" -DBU_DESKTOP -std=gnu++17 ../Bibliotekum_Ultimatum/Binaries/SDL_win/SDL2main.lib ../Bibliotekum_Ultimatum/Binaries/SDL_win/SDL2.lib -lopengl32 -lshell32 -m32 -O3 -Xlinker /subsystem:windows
echo if exist "%%cur%%\..\..\Compiled\Windows\%appName%_resources" rmdir /S /Q %%cur%%\..\..\Compiled\Windows\%appName%_resources
echo robocopy %%cur%%\..\%appName%_resources %%cur%%\..\..\Compiled\Windows\%appName%_resources /E
) > %cur%\%appName%\main\Compilation_scripts\make-final.bat





if "%typProj%" == "0"	copy %cur%\..\Link\Bibliotekum_Ultimatum\IDEal\EmptyProjects\Blank\main.cpp %cur%\%appName%\main\main.cpp
if "%typProj%" == "0"	copy %cur%\..\Link\Bibliotekum_Ultimatum\IDEal\EmptyProjects\Blank\header.hpp %cur%\%appName%\main\header.hpp

if "%typProj%" == "1"	copy %cur%\..\Link\Bibliotekum_Ultimatum\IDEal\EmptyProjects\Basic\main.cpp %cur%\%appName%\main\main.cpp
if "%typProj%" == "1"	copy %cur%\..\Link\Bibliotekum_Ultimatum\IDEal\EmptyProjects\Basic\header.hpp %cur%\%appName%\main\header.hpp

if "%typProj%" == "2"	copy %cur%\..\Link\Bibliotekum_Ultimatum\IDEal\EmptyProjects\Basic3D\main.cpp %cur%\%appName%\main\main.cpp
if "%typProj%" == "2"	copy %cur%\..\Link\Bibliotekum_Ultimatum\IDEal\EmptyProjects\Basic3D\header.hpp %cur%\%appName%\main\header.hpp

pause