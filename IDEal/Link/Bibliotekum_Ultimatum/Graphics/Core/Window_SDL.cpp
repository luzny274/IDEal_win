#ifndef BU_WINDOW_SDL_IMPL
#define BU_WINDOW_SDL_IMPL

#include "../SDL2/include/SDL.h"
#include "Core.hpp"

#ifndef NOT_BU_PROJECT

void ulm::Window::initialize(const char* title, int w, int h){
    SDL_GLContext context;

    SDL_Init( SDL_INIT_EVERYTHING );    

    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 4 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 4 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 4 );
    SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 4 );
    SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE, 32 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    #ifdef BU_MOBILE
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    #else
        #ifndef FORCE_3_1
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        #else
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        #endif
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    #endif

    #ifdef BU_MOBILE
        window = (void *)SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
    #else
        window = (void *)SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL);
    #endif
    
    setResizable(true);
    
    if (!window) {
        String error("Couldn't create window: ");
        error += SDL_GetError();
        Properties::handleError(error);
        return;
    }

    context = SDL_GL_CreateContext((SDL_Window*)window);
    if (!context) {
        String error("Couldn't create context: ");
        error += SDL_GetError();
        Properties::handleError(error);
        return;
    }



    #ifdef BU_MOBILE
        if (!gladLoadGLES2Loader((GLADloadproc)SDL_GL_GetProcAddress))
        {
            Properties::handleError("Failed to initialize GLAD");
            return;
        }
    #else
        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
        {
            Properties::handleError("Failed to initialize GLAD");
            return;
        }
    #endif

    ulm::DefaultFrameBuffer::w = w;
    ulm::DefaultFrameBuffer::h = h;
    
    glViewport(0, 0, w, h);
    ulm::Window::width = w;
    ulm::Window::height = h;

    glm::ivec2 size = getScreenSize();
    screenSize = size;

    #ifndef BU_MOBILE
        glGenVertexArrays(1, &vertexArray);
        glBindVertexArray(vertexArray);
    #else
        glViewport(0, 0, size.x, size.y);
        ulm::Window::width = size.x;
        ulm::Window::height = size.y;
    #endif

    setVSync(true);
}
#endif

void ulm::Window::resizeGL(int w, int h){
    glViewport(DefaultFrameBuffer::x, DefaultFrameBuffer::y, w, h);
    ulm::Window::width = w;
    ulm::Window::height = h;
    DefaultFrameBuffer::w = w;
    DefaultFrameBuffer::h = h;
}

void ulm::Window::swap(){
    SDL_GL_SwapWindow((SDL_Window* )window);
}

void ulm::Window::warpMouse(int x, int y){
    SDL_WarpMouseInWindow((SDL_Window*) window, x, y);
}

void ulm::Window::warpMouseInCenter(){
    warpMouse(width/2, height/2);
}

void ulm::Window::showCursor(bool show){
    SDL_ShowCursor(0); 
}

char * ulm::Window::getClipboardText(){
    return(SDL_GetClipboardText());
}

void ulm::Window::setClipboardText(const char * text){
    SDL_SetClipboardText(text);
}

void ulm::Window::setVSync(bool enabled){
    if(enabled)
        SDL_GL_SetSwapInterval(1);
    else
        SDL_GL_SetSwapInterval(0);
}

#ifdef BU_MOBILE
    void ulm::Window::setResizable(bool enabled){
    }
#else
    void ulm::Window::setResizable(bool enabled){
        if(enabled)
            SDL_SetWindowResizable((SDL_Window* )window, SDL_TRUE);
        else
            SDL_SetWindowResizable((SDL_Window* )window, SDL_FALSE);
    }
#endif


#ifdef BU_MOBILE
    void ulm::Window::setWindowMode(ulm::WindowMode mode){
    }
#else
    void ulm::Window::setWindowMode(ulm::WindowMode mode){
        if(mode == ulm::FULLSCREEN)
            SDL_SetWindowFullscreen((SDL_Window* )window,SDL_WINDOW_FULLSCREEN);
        else if(mode == ulm::FULLSCREEN_DESKTOP)
            SDL_SetWindowFullscreen((SDL_Window* )window,SDL_WINDOW_FULLSCREEN_DESKTOP);
        else if(mode == ulm::WINDOWED)
            SDL_SetWindowFullscreen((SDL_Window* )window,0);
    }
#endif

#ifdef BU_MOBILE
    void ulm::Window::maximize(bool enabled){}
#else
    void ulm::Window::maximize(bool enabled){
        if(enabled)
            SDL_MaximizeWindow((SDL_Window* ) window);
        else
            SDL_RestoreWindow((SDL_Window* ) window);

        getScreenSize();
        ulm::Window::width = ulm::Window::screenSize.x;
        ulm::Window::height = ulm::Window::screenSize.y;
    }
#endif


glm::ivec2 ulm::Window::getScreenSize(){
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    screenSize = glm::ivec2(DM.w, DM.h);
    return(glm::ivec2(DM.w, DM.h));
}

void ulm::Window::setRelativeMouseMode(bool enabled){
    if(enabled){
        SDL_SetRelativeMouseMode(SDL_TRUE);
        ulm::Window::mouse.relativeMode = true;
    }
    else{
        SDL_SetRelativeMouseMode(SDL_FALSE);
        ulm::Window::mouse.relativeMode = false;
    }
}

#endif