set cur=%0\..
clang++ ../main.cpp -o ../../Compiled/Windows/Example_animation.exe -DBU_SDL -DBU_WINDOWS -DBU_APP_NAME=\"Example_animation\" -DBU_DESKTOP -std=gnu++17 ../Bibliotekum_Ultimatum/Binaries/SDL_win/SDL2main.lib ../Bibliotekum_Ultimatum/Binaries/SDL_win/SDL2.lib -lopengl32 -lshell32 -m32 -O3 -Xlinker /subsystem:windows
if exist "%cur%\..\..\Compiled\Windows\Example_animation_resources" rmdir /S /Q %cur%\..\..\Compiled\Windows\Example_animation_resources
robocopy %cur%\..\Example_animation_resources %cur%\..\..\Compiled\Windows\Example_animation_resources /E
