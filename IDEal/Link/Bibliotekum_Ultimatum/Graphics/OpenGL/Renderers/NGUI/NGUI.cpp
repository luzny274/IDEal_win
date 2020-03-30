#ifndef NGUI_BU
#define NGUI_BU

#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024

#include "nk_impl.h"
#include "../../../Core/Core.hpp"

namespace ulm{
    class NK{
        private:
            static int w;
            static int h;
            static bool initialized;

        public:
            static struct nk_context *ctx;

            static void initialize(){
                if(!initialized){
                    initialize(Window::width, Window::height);
                }
            }

            static void initialize(int width, int height){
                if(!initialized){
                    setSize(width, height);
                    ctx = nk_sdl_init();

                    struct nk_font_atlas *atlas;
                    nk_sdl_font_stash_begin(&atlas);
                    nk_sdl_font_stash_end();
                    initialized = true;
                }
            }

            static void initialize(int width, int height, int fontHeight){
                if(!initialized){
                    setSize(width, height);
                    ctx = nk_sdl_init();

                    struct nk_font_atlas *atlas;
                    nk_sdl_font_stash_begin(&atlas);
                    nk_sdl_font_stash_end(fontHeight);
                    initialized = true;
                }
            }

            static void setSize(glm::vec2 size){
                setSize((int)size.x, (int)size.y);
            }

            static void setSize(glm::ivec2 size){
                setSize(size.x, size.y);
            }

            static void setSize(int width, int height){
                w = width;
                h = height;
            }

            static void beginInput(){
                nk_input_begin(ctx);
            }

            static void endInput(){
                nk_input_end(ctx);
            }
            
            static void processInput(  ulm::MOUSE mouse,
                                ulm::WHEEL wheel,
                                char * keysPressed, char * keysDown, char * keysUp,
                                char * textInput){
                initialize();

                nk_input_begin(ctx);
                nk_handle_input(mouse,
                                wheel,
                                keysPressed, keysDown, keysUp,
                                textInput, w, h);
                nk_input_end(ctx);
            }

            static void draw(){
                initialize();
                glDisable(GL_CULL_FACE);  
                glDisable(GL_STENCIL_TEST);
                glDisable(GL_DEPTH_TEST);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY, w, h);
            }
    };

    int NK::w = 0;
    int NK::h = 0;
    bool NK::initialized = false;
    struct nk_context* NK::ctx = NULL;

}


#endif