clang++ ../main.cpp -o ../test.exe -DBU_SDL -DBU_WINDOWS -DBU_APP_NAME=\"Example_3D\" -DBU_DESKTOP -std=gnu++17 ../Bibliotekum_Ultimatum/Binaries/SDL_win/SDL2main.lib ../Bibliotekum_Ultimatum/Binaries/SDL_win/SDL2.lib -lopengl32 -lshell32 -m32 -Xlinker /subsystem:console
cd ..
test.exe
