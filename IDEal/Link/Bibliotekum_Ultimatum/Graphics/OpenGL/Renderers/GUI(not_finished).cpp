#ifndef BU_ULM_GUI
#define BU_ULM_GUI

#include "../../../Base/base.hpp"
#include "../Base/OpenGL.h"
#include "../../../Tools/Tools.hpp"
#include "../Tools/ToolsGL.hpp"

namespace ulm{
    class GUIElement{
        public:
            float x = 0.0f;
            float y = 0.0f;
            float w = 0.0f;
            float h = 0.0f;
            glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
            glm::vec4 backgroundColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

            glm::vec4 data1 = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
            glm::vec4 data2 = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

            String code = "";

            virtual void hover(GUIElement * e, float x, float y) = 0;
            virtual void LClick(GUIElement * e, float x, float y) = 0;
            virtual void LClicked(GUIElement * e, float x, float y) = 0;
    };

    class Canvas : public GUIElement{
        public:
            Canvas(){}
            Canvas(float X, float Y, float W, float H, glm::vec4 COLOR){
                initialize(X, Y, W, H, COLOR);
            }
            void initialize(float X, float Y, float W, float H, glm::vec4 COLOR){
                code = R"====( 
                    glFragColor = color[index];
                )====";

                x = X;
                y = Y;
                w = W;
                h = H;
                color = COLOR;
            }

            void hover(GUIElement * e, float x, float y){}
            void LClick(GUIElement * e, float x, float y){}
            void LClicked(GUIElement * e, float x, float y){}
    };

    

    class Panel{
        private:
            Array<String> codes;

            Shader shader;
            VertexArray va;

            Canvas canvas;

        public:
            Array<GUIElement*> elements;
            Array<int> IDs;

            glm::mat4 modelMat = glm::mat4(1.0f);
            Camera * camera = NULL;

            Panel(){}

            void add(GUIElement * e){
                int i = codes.indexOf(e->code);
                if(i == -1){
                    codes.add(e->code);
                    IDs.add(codes.size - 1);
                    elements.add(e);
                }else{
                    IDs.add(i);
                    elements.add(e);
                }
                createShader();
            }

            Panel(float X, float Y, float W, float H, glm::vec4 COLOR){
                initialize(X, Y, W, H, COLOR);
            }

            void initialize(float X, float Y, float W, float H, glm::vec4 COLOR){
                canvas.initialize(X, Y, W, H, COLOR);
                add(&canvas);
            }

            glm::mat4 getMVP(){
                if(camera == NULL) return(modelMat);
                else return(modelMat * camera->getVP());
            }

            void createShader(){
                String vertSource = String(R"====(#version 100
                                        #ifdef GL_ES
                                            precision mediump float;
                                        #endif
                                        )====") + "#define ELEMENT_COUNT " + elements.size + R"====(
                                        
                                        attribute float BU_type;
                                        attribute float BU_Index;
                                        attribute float BU_ID;

                                        varying vec2 position;
                                        varying float ID;
                                        varying float index;

                                        uniform float x[ELEMENT_COUNT];
                                        uniform float y[ELEMENT_COUNT];
                                        uniform float w[ELEMENT_COUNT];
                                        uniform float h[ELEMENT_COUNT];
                                        uniform mat4 BU_MVP;

                                        void main()
                                        {
                                            index = BU_Index;
                                            ID = BU_ID;

                                            int i = int(index);

                                            if(BU_type == 0.0){
                                                position = vec2(-1.0, -1.0);                                                
                                                gl_Position = BU_MVP * vec4(x[i], y[i], 0.0, 1.0);

                                            }else if(BU_type == 1.0){

                                                position = vec2(1.0, -1.0);
                                                gl_Position = BU_MVP * vec4(x[i] + w[i], y[i], 0.0, 1.0);

                                            }else if(BU_type == 2.0){

                                                position = vec2(-1.0, 1.0);
                                                gl_Position = BU_MVP * vec4(x[i], y[i] + h[i], 0.0, 1.0);

                                            }else if(BU_type == 3.0){

                                                position = vec2(1.0, 1.0);
                                                gl_Position = BU_MVP * vec4(x[i] + w[i], y[i] + h[i], 0.0, 1.0);

                                            }

                                        }
                                        )====";


                String fragSource = String(R"====(#version 100
                                        #ifdef GL_ES
                                            precision mediump float;
                                        #endif
                                        )====") + "#define ELEMENT_COUNT " + elements.size + R"====(
                                                            
                                        
                                        uniform float x[ELEMENT_COUNT];
                                        uniform float y[ELEMENT_COUNT];
                                        uniform float w[ELEMENT_COUNT];
                                        uniform float h[ELEMENT_COUNT];
                                        uniform vec4 color[ELEMENT_COUNT];
                                        uniform vec4 backgroundColor[ELEMENT_COUNT];
                                        uniform vec4 data1[ELEMENT_COUNT];
                                        uniform vec4 data2[ELEMENT_COUNT];

                                        varying vec2 position;
                                        varying float ID;
                                        varying float index;

                                        void main(){
                                            gl_FragColor = color[int(index)];
                                        }

                                    )====";

                shader.initialize(vertSource.getPtr(), fragSource.getPtr());

                va.initializeBuffers();
                int attr[] = {1, 1, 1};
                Array<float> VB;
                Array<unsigned int> IB;

                for(int i = 0; i < elements.size; i++){
                    VB += {0.0f, (float)i, (float)IDs[i]};
                    VB += {1.0f, (float)i, (float)IDs[i]};
                    VB += {2.0f, (float)i, (float)IDs[i]};
                    VB += {3.0f, (float)i, (float)IDs[i]};

                    IB += 0 + i * 3;
                    IB += 1 + i * 3;
                    IB += 2 + i * 3;
                    IB += 3 + i * 3;
                    IB += 2 + i * 3;
                    IB += 1 + i * 3;
                }

                va.setVertexBuffer(VB.getPtr(), VB.size * sizeof(float), attr, 3);
                va.setIndexBuffer(IB.getPtr(), IB.size * sizeof(unsigned int));
            }


            void draw(){
                shader.bind();
                va.bind(true);

                shader.setMat4("BU_MVP", getMVP());

                for(int i = 0; i < elements.size; i++){
                    shader.setFloat(String("x[") + i + "]", elements[i]->x);
                    shader.setFloat(String("y[") + i + "]", elements[i]->y);
                    shader.setFloat(String("w[") + i + "]", elements[i]->w);
                    shader.setFloat(String("h[") + i + "]", elements[i]->h);


                    shader.setVec4(String("backgroundColor[") + i + "]", elements[i]->backgroundColor);
                    shader.setVec4(String("data1[") + i + "]", elements[i]->data1);
                    shader.setVec4(String("data2[") + i + "]", elements[i]->data2);
                    shader.setVec4(String("color[") + i + "]", elements[i]->color);
                }

                va.draw();
            }

            float& getX(){
                return(canvas.x);
            }

            float& getY(){
                return(canvas.y);
            }

            float& getW(){
                return(canvas.w);
            }

            float& getH(){
                return(canvas.h);
            }
    };

    class GUIControls{
        private:
            Array<Panel*> panels;
            GUIElement * clicked = NULL;

        public:            
            void add(Panel* panel){
                panels.add(panel);
            }
            void processInput(int x, int y, bool LClick, bool RClick, bool MClick){
                bool find = true;

                if(!LClick && clicked != NULL){
                    clicked->LClicked((float)x, (float)y);
                    clicked = NULL;
                }
                else if(clicked != NULL){
                    find = false;
                    clicked->LClick((float)x, (float)y);
                }


                bool found = false;

                for(int i = panels.size - 1; i >= 0; i--){
                    for(int ii = panels[i]->elements.size - 1; ii >= 0; ii--){
                        GUIElement * el = panels[i]->elements[ii];

                        glm::vec4 pos1 = panels[i]->getMVP() * glm::vec4(el->x, el->y, 0.0, 1.0);
                        glm::vec4 pos2 = panels[i]->getMVP() * glm::vec4(el->x + el->w, el->y + el->h, 0.0, 1.0);

                        pos1.y *= -1.0f; pos2.y *= -1.0f;

                        int x1 = (int)((pos1.x + 1.0f) / 2.0f * (float)Window.width);
                        int y1 = (int)((pos1.y + 1.0f) / 2.0f * (float)Window.height);

                        int x2 = (int)((pos2.x + 1.0f) / 2.0f * (float)Window.width);
                        int y2 = (int)((pos2.y + 1.0f) / 2.0f * (float)Window.height);

                        bool c = false;

                        if(x < x1 + x2 && x > x1 && x > ){

                        }
                    }
                    //if(found) break;
                }
            }
            void draw(){
                for(int i = 0; i < panels.size; i++){
                    panels[i]->draw();
                }
            }
    };
}

#endif