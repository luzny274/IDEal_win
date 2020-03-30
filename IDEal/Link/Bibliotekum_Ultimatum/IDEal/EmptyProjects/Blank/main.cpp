#include "header.hpp"

class Main : public ulm::Program{
    public:
        Main(){
            /* Initialization */
        }

        void update(float deltaTime){
            /* Run each frame */
        }

        void render(ulm::Eye eye){
            /* Render frame */
        }

        void resizeCallback(int width, int height){
            /* When resizing */
        }
        
        ~Main(){
            /* Destruction */
        }
};

ulm::Program * ulm::Properties::onStart(){
    ulm::Window::initialize(BU_APP_NAME, 256, 256);
    ulm::Window::maximize(true);
    return new Main();
}

void ulm::Properties::handleError(ulm::String error){
    printf("\n%s\n", error.getPtr());
    fflush(stdout);
}