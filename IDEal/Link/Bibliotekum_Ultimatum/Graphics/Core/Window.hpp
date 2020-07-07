#ifndef WINDOW_BU
#define WINDOW_BU

#include "../../Base/base.hpp"
#include "../OpenGL/Base/OpenGL.h"
#include "Window_Keys.cpp"

#define ULM_UTF_SIZE 4

#ifndef MAX_CONTROLLERS
#define MAX_CONTROLLERS 4
#endif

namespace ulm{

    enum WindowMode   { WINDOWED = 0, FULLSCREEN = 1, FULLSCREEN_DESKTOP = 2 };
    struct MOUSE      { int x, y, dx, dy; bool relativeMode;                 };
    struct WHEEL      { int dx, dy;                                          };

    struct CONTROLLER { bool isPlugged = false;
                        float lX, lY;
                        float rX, rY;
                        float triggerLeft, triggerRight;
                        bool up;
                        bool down;          
                        bool left;          
                        bool right;         
                        bool start;         
                        bool back;          
                        bool leftShoulder;  
                        bool rightShoulder; 
                        bool A;       
                        bool B;       
                        bool X;       
                        bool Y;};

    class Window{
        private:
            static char lastKeysPressed[NUM_SCANCODES];

        public:
            static unsigned int vertexArray;
            static void * window;
            static int width;
            static int height;
            static glm::ivec2 screenSize;
            static glm::vec3 clearColor;

            static MOUSE mouse;
            static WHEEL wheel;
            static CONTROLLER controllers[MAX_CONTROLLERS];

            static char keysPressed[NUM_SCANCODES], keysDown[NUM_SCANCODES], keysUp[NUM_SCANCODES];
            static char textInput[ULM_UTF_SIZE];

            #ifndef NOT_BU_PROJECT
            static void initialize(const char* title, int w, int h);
            #endif

            static void resizeGL(int width, int height);
            static void swap();

            static void warpMouse(int x, int y);
            static void warpMouseInCenter();
            static void showCursor(bool show);

            static char * getClipboardText();
            static void setClipboardText(const char * text);

            static void updateKeys(){
                for(int i = 0; i < NUM_SCANCODES; i++){
                    if(lastKeysPressed[i] == 0 && keysPressed[i] == 1) keysDown[i] = 1;
                    else keysDown[i] = 0;

                    if(lastKeysPressed[i] == 1 && keysPressed[i] == 0) keysUp[i] = 1;
                    else keysUp[i] = 0;

                    lastKeysPressed[i] = keysPressed[i];
                }
            }

            static bool controllerExists(){
                for(int i = 0; i < MAX_CONTROLLERS; i++)
                    if(controllers[i].isPlugged)
                        return true;

                return false;
            }

            static CONTROLLER getFirstPluggedController(){
                for(int i = 0; i < MAX_CONTROLLERS; i++){
                    if(controllers[i].isPlugged)
                        return controllers[i];
                }

                return controllers[0];
            }
            
            static CONTROLLER getLastPluggedController(){
                for(int i = MAX_CONTROLLERS - 1; i >= 0; i--){
                    if(controllers[i].isPlugged)
                        return controllers[i];
                }

                return controllers[0];
            }

            static void exit();
            static void setVSync(bool enabled);
            static void setWindowMode(WindowMode mode);
            static void maximize(bool enabled);
            static void setResizable(bool enabled);
            static void setRelativeMouseMode(bool enabled);
            static glm::ivec2 getScreenSize();

            static bool getGLError(bool kill);

    };

    glm::vec3       Window::clearColor = glm::vec3(0.0f);
    glm::ivec2      Window::screenSize;
    unsigned int    Window::vertexArray = 0;
    char            Window::lastKeysPressed[NUM_SCANCODES] = {0};
    void *          Window::window = NULL;
    int             Window::width = 0;
    int             Window::height = 0;
    MOUSE           Window::mouse = {0, 0, 0, 0};
    WHEEL           Window::wheel = {0, 0};
    CONTROLLER      Window::controllers[MAX_CONTROLLERS];
    char            Window::keysPressed[NUM_SCANCODES] = {0};
    char            Window::keysDown[NUM_SCANCODES] = {0};
    char            Window::keysUp[NUM_SCANCODES] = {0};
    char            Window::textInput[ULM_UTF_SIZE] = {0, 0, 0, 0};

}


bool ulm::Window::getGLError(bool kill){
    GLenum error = glGetError();
    bool end = false;
    while(error != GL_NO_ERROR){
        char mess[512];
        sprintf(mess, "%X / %d", error, error);
        String err = "OpenGL error: ";
        err+=mess;
        if(error == GL_INVALID_OPERATION)
            err+="::GL_INVALID_OPERATION";
        if(error == GL_INVALID_ENUM)
            err+="::GL_INVALID_ENUM";
        if(error == GL_INVALID_VALUE)
            err+="::GL_INVALID_VALUE";

        Properties::handleError(err);
        
        error = glGetError();
        end = true;
    }

    if(end && kill) exit();

    return(end);
}


#endif