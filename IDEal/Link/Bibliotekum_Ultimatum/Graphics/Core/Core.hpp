#ifndef BU_GRAPHICS_INDEPENDENT_TOOLS
#define BU_GRAPHICS_INDEPENDENT_TOOLS

/* Inclusion */
#include "ProgramStructure.cpp"
#include "Properties.cpp"
#include "Window.hpp"

#include "../OpenGL/Renderers/NGUI/NGUI.cpp"

#ifdef BU_SDL
    #include "Window_SDL.cpp"
    #include "SDL_MAIN.cpp"
#endif

#ifdef BU_ANDROID
    #include "Window_Android.cpp"
    #include "Android_JNI.cpp"
#endif

#endif