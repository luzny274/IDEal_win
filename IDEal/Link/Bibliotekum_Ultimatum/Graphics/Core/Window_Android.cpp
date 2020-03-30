#ifndef BU_WINDOW_SDL_IMPL
#define BU_WINDOW_SDL_IMPL

#include <jni.h>

void ulm::Window::initialize(const char* title, int w, int h){
    ulm::Window::width = w;
    ulm::Window::height = h;

    ulm::Window::screenSize.x = w;
    ulm::Window::screenSize.y = h;
}

void ulm::Window::swap(){
}

void ulm::Window::warpMouse(int x, int y){}

void ulm::Window::warpMouseInCenter(){}

void ulm::Window::showCursor(bool show){}

char * ulm::Window::getClipboardText(){return(NULL);}

void ulm::Window::setClipboardText(const char * text){}

void ulm::Window::setVSync(bool enabled){
}

void ulm::Window::setResizable(bool enabled){}
void ulm::Window::setWindowMode(ulm::WindowMode mode){}
void ulm::Window::maximize(bool enabled){}

void ulm::Window::resizeGL(int width, int height){

}
void ulm::Window::exit(){}
void ulm::Window::setRelativeMouseMode(bool enabled){}


glm::ivec2 ulm::Window::getScreenSize(){
    return(screenSize);
}

#endif