#ifndef BU_SDL_MAIN_CPP
#define BU_SDL_MAIN_CPP

#ifdef BU_DESKTOP
#define SDL_MAIN_HANDLED
#endif

#include "../SDL2/include/SDL.h"

#include "Core.hpp"


SDL_bool done = SDL_FALSE;

#ifndef MAX_CONTROLLERS
#define MAX_CONTROLLERS 4
#endif

SDL_GameController *ControllerHandles[MAX_CONTROLLERS] = {NULL};

void initJoysticks(){
    for(int ControllerIndex = 0; ControllerIndex < MAX_CONTROLLERS; ++ControllerIndex)
        if (ControllerHandles[ControllerIndex]){
            SDL_GameControllerClose(ControllerHandles[ControllerIndex]);
            ControllerHandles[ControllerIndex] = NULL;
        }


    int MaxJoysticks = SDL_NumJoysticks();
    int ControllerIndex = 0;
    for(int JoystickIndex = 0; JoystickIndex < MaxJoysticks; ++JoystickIndex)
    {
        if (!SDL_IsGameController(JoystickIndex))
            continue;
        if (ControllerIndex >= MAX_CONTROLLERS)
            break;

        ControllerHandles[ControllerIndex] = SDL_GameControllerOpen(JoystickIndex);
        ControllerIndex++;
    }
}

void getControllersInput(){
    for (int ControllerIndex = 0;
        ControllerIndex < MAX_CONTROLLERS;
        ++ControllerIndex)
    {
        if(ControllerHandles[ControllerIndex] != 0 && SDL_GameControllerGetAttached(ControllerHandles[ControllerIndex]))
        {
            ulm::Window::controllers[ControllerIndex].isPlugged = true;
            ulm::Window::controllers[ControllerIndex].up =               SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_DPAD_UP);
            ulm::Window::controllers[ControllerIndex].down =             SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_DPAD_DOWN);
            ulm::Window::controllers[ControllerIndex].left =             SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_DPAD_LEFT);
            ulm::Window::controllers[ControllerIndex].right =            SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
            ulm::Window::controllers[ControllerIndex].start =            SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_START);
            ulm::Window::controllers[ControllerIndex].back =             SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_BACK);
            ulm::Window::controllers[ControllerIndex].leftShoulder =     SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
            ulm::Window::controllers[ControllerIndex].rightShoulder =    SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
            ulm::Window::controllers[ControllerIndex].A =                SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_A);
            ulm::Window::controllers[ControllerIndex].B =                SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_B);
            ulm::Window::controllers[ControllerIndex].X =                SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_X);
            ulm::Window::controllers[ControllerIndex].Y =                SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_Y);

            ulm::Window::controllers[ControllerIndex].lX =            (float)SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_LEFTX)           / 32767.f;
            ulm::Window::controllers[ControllerIndex].lY =           -(float)SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_LEFTY)           / 32767.f;
            ulm::Window::controllers[ControllerIndex].rX =            (float)SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_RIGHTX)          / 32767.f;
            ulm::Window::controllers[ControllerIndex].rY =            (float)SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_RIGHTY)          / 32767.f;
            ulm::Window::controllers[ControllerIndex].triggerLeft  =  (float)SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_TRIGGERLEFT)     / 32767.f;
            ulm::Window::controllers[ControllerIndex].triggerRight =  (float)SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_TRIGGERRIGHT)    / 32767.f;
        }
        else
            ulm::Window::controllers[ControllerIndex].isPlugged = false;
    }
}

int main(int argc, char* argv[]){
    const Uint8 * SDL_keys;
    float currentTime = 0.0f, deltaTime = 0.0f, last = 0.0f;
    char emptyWchar[ULM_UTF_SIZE] = {0, 0, 0, 0};

    SDL_Event event;

    ulm::Window::mouse.relativeMode = false;
    ulm::Program * currentProgram = ulm::Properties::onStart();

    initJoysticks();

    while (!done) {
        ulm::DefaultFrameBuffer::bind();

        if(ulm::Properties::change){
            ulm::Properties::change = false;
            delete currentProgram;
            currentProgram = ulm::Properties::current;
        }

        currentTime = (float)SDL_GetTicks()/(float)1000;
        deltaTime = currentTime - last;
        last = currentTime;


        ulm::Window::swap();        
        glClearColor(ulm::Window::clearColor.x, ulm::Window::clearColor.y, ulm::Window::clearColor.z, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        SDL_keys = SDL_GetKeyboardState(NULL);

        for(int i = 0; i < ulm::SCANCODE_LCLICK; i++) ulm::Window::keysPressed[i] = SDL_keys[i];
        memcpy(ulm::Window::textInput, emptyWchar, NK_UTF_SIZE);
        ulm::Window::wheel.dx = 0;
        ulm::Window::wheel.dy = 0;
        ulm::Window::mouse.dx = 0;
        ulm::Window::mouse.dy = 0;

        getControllersInput();

        while (SDL_PollEvent(&event)) {
            switch(event.type){
                case SDL_QUIT:
                    done = SDL_TRUE;
                    break;
                
                case SDL_WINDOWEVENT:
                    if(event.window.event == SDL_WINDOWEVENT_RESIZED){                                
                        ulm::Window::resizeGL(event.window.data1, event.window.data2);
                        currentProgram->resizeCallback(event.window.data1, event.window.data2);
                    }
                    break;

                case SDL_MOUSEMOTION:
                    ulm::Window::mouse.x =  event.motion.x;
                    ulm::Window::mouse.y =  event.motion.y;
                    ulm::Window::mouse.dx = event.motion.xrel;
                    ulm::Window::mouse.dy = event.motion.yrel;
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if(event.button.button == SDL_BUTTON_LEFT)   ulm::Window::keysPressed[ulm::SCANCODE_LCLICK] = 1;
                    if(event.button.button == SDL_BUTTON_RIGHT)  ulm::Window::keysPressed[ulm::SCANCODE_RCLICK] = 1;
                    if(event.button.button == SDL_BUTTON_MIDDLE) ulm::Window::keysPressed[ulm::SCANCODE_MCLICK] = 1;
                    break;

                case SDL_MOUSEBUTTONUP:
                    if(event.button.button == SDL_BUTTON_LEFT)   ulm::Window::keysPressed[ulm::SCANCODE_LCLICK] = 0;
                    if(event.button.button == SDL_BUTTON_RIGHT)  ulm::Window::keysPressed[ulm::SCANCODE_RCLICK] = 0;
                    if(event.button.button == SDL_BUTTON_MIDDLE) ulm::Window::keysPressed[ulm::SCANCODE_MCLICK] = 0;
                    break;

                case SDL_TEXTINPUT:
                    memcpy(ulm::Window::textInput, event.text.text, NK_UTF_SIZE);
                    break;

                case SDL_MOUSEWHEEL:
                    ulm::Window::wheel.dx = event.wheel.x;
                    ulm::Window::wheel.dy = event.wheel.y;
                    break;

                case SDL_CONTROLLERDEVICEADDED:
                case SDL_CONTROLLERDEVICEREMOVED:
                case SDL_CONTROLLERDEVICEREMAPPED:
                    initJoysticks();
                    break;
            }
            
        }
        ulm::Window::updateKeys();

        currentProgram->update(deltaTime);
        currentProgram->render(ulm::nullEye);


    }

    delete currentProgram;


    for(int ControllerIndex = 0; ControllerIndex < MAX_CONTROLLERS; ++ControllerIndex)
        if (ControllerHandles[ControllerIndex])
            SDL_GameControllerClose(ControllerHandles[ControllerIndex]);

    SDL_Quit();
    return(1);    
}

void ulm::Window::exit(){
    done = SDL_TRUE;
}

#endif