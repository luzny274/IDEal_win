#include "header.hpp"

class Main : public ulm::Program{
    public:
        float rotation = 0.0f;
        float time = 0.0f;
        float dt = 0.0f;
        long int numberOfFrames = 0;
        ulm::Sprite sprite;

        Main(){
            /* Initialization */

            ulm::Window::setVSync(false);

            sprite = ulm::SpriteFactory::createRectangle(-0.5f, -0.5f, 1.0f, 1.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        }

        void update(float deltaTime){
            /* Run each frame */

            sprite.modelMat = ulm::Math::rotate2D(glm::mat4(1.0f), rotation);
            numberOfFrames++;
            time += deltaTime;
            dt = deltaTime;
            
            if(ulm::Window::keysDown[ulm::SCANCODE_ESCAPE])
                ulm::Window::exit();
        }

        void render(ulm::Eye eye){
            /* Render frame */

            /* 2D */

            ulm::Renderer2D::begin(glm::mat4(1.0f));
            ulm::Renderer2D::draw(sprite, ulm::Colored);
            ulm::Renderer2D::end();

            /* GUI */

            ulm::NK::processInput(ulm::Window::mouse, ulm::Window::wheel,
                                  ulm::Window::keysPressed, ulm::Window::keysDown, ulm::Window::keysUp, 
                                  ulm::Window::textInput);

            if (nk_begin(ulm::NK::ctx, "Demo", nk_rect(20, 20, 300, 200),
                NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
                NK_WINDOW_CLOSABLE|NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
            {
                nk_layout_row_static(ulm::NK::ctx, 30, 80, 1);
                if (nk_button_label(ulm::NK::ctx, "click")){
                    printf("clicked\n"); 
                    fflush(stdout);
                }
                nk_layout_row_dynamic(ulm::NK::ctx, 20, 1); 
                nk_label_wrap(ulm::NK::ctx, (ulm::String("current dt: ") + dt + " ms").getPtr());

                nk_layout_row_dynamic(ulm::NK::ctx, 20, 1); 
                nk_label_wrap(ulm::NK::ctx, (ulm::String("average dt: ") + (time / (float)numberOfFrames) + " ms").getPtr());
                nk_layout_row_dynamic(ulm::NK::ctx, 20, 1); 
                nk_label_wrap(ulm::NK::ctx, (ulm::String("average framerate: ") + ((float)numberOfFrames / time) + " fps").getPtr());

                nk_layout_row_dynamic(ulm::NK::ctx, 40, 1);
                nk_slider_float(ulm::NK::ctx, 0.0f, &rotation, M_PI * 2.0f, 0.01f);
            }

            nk_end(ulm::NK::ctx);
            ulm::NK::draw();
        }

        void resizeCallback(int width, int height){
            /* When resize */
            ulm::NK::setSize(width, height);
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