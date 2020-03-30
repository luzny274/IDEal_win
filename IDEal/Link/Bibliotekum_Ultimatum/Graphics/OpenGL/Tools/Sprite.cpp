#ifndef SPRITE_BU
#define SPRITE_BU

#include "ToolsGL.hpp"

namespace ulm{
    class Sprite{
        private:
            
        public:
            VertexArray vertexArray;
            Texture texture;

            Array<glm::vec2> vertices;
            Array<glm::vec2> texture_coordinates;
            Array<glm::vec4> colors;
            Array<unsigned int> indices;

            glm::mat4 modelMat = glm::mat4(1.0f);
            bool initialized = false;

            glm::vec4 colorMultiplier = glm::vec4(1.f);

            void initialize(){ initialize(true); }

            void initialize(bool freeMemory){
                initialized = true;
                
                vertexArray.initializeBuffers();

                ByteArray VB;

                if(vertices.isEmpty())
                    Properties::handleError("ULM::ERROR::Sprite::initialize::no vertices found");

                if(colors.isEmpty())
                    for(int i = 0; i < vertices.size; i++)
                        colors.add(glm::vec4(1.0f));
                
                if(texture_coordinates.isEmpty())
                    for(int i = 0; i < vertices.size; i++)
                        texture_coordinates.add(glm::vec2(0.f));


                for(int i = 0; i < vertices.size; i++){
                    VB.add(vertices[i].x);
                    VB.add(vertices[i].y);

                    VB.add(colors[i].x);
                    VB.add(colors[i].y);
                    VB.add(colors[i].z);
                    VB.add(colors[i].w);

                    VB.add(texture_coordinates[i].x);
                    VB.add(texture_coordinates[i].y);
                }//*/
                /*for(int i = 0; i < vertices.size; i++){
                    VB.add(-0.5f);
                    VB.add(vertices[i].y);

                    VB.add(colors[i].x);
                    VB.add(colors[i].y);
                    VB.add(colors[i].z);
                    VB.add(colors[i].w);

                    VB.add(texture_coordinates[i].x);
                    VB.add(texture_coordinates[i].y);
                }//*/

                Map<int, Type> attr = Map<int,Type>({2, 4, 2}, FLOAT);
                vertexArray.setBuffers(indices, VB, attr);
                
                if(freeMemory){
                    vertices.empty();
                    texture_coordinates.empty();
                    colors.empty();
                }
            }

            void freeMemory(){
                vertices.empty();
                texture_coordinates.empty();
                colors.empty();
            }

            void copy(const Sprite& other){
                vertexArray=other.vertexArray;
                texture=other.texture;
                vertices=other.vertices;
                texture_coordinates=other.texture_coordinates;
                initialized=other.initialized;
                colors=other.colors;
                indices=other.indices;
                modelMat=other.modelMat;
            }


            Sprite(const Sprite& other){
                copy(other);
            }

            Sprite& operator=(const Sprite& other){
                copy(other);
                return(*this);
            }

            Sprite(){}
    };


    class SpriteFactory{
        public:

            static Sprite createRectangle(float x, float y, float width, float height){                             return createRectangle(x, y, width, height, glm::vec4(1.f), false); }            
            static Sprite createRectangle(float x, float y, float width, float height, glm::vec4 color){            return createRectangle(x, y, width, height, color, false); }
            static Sprite createRectangle(float x, float y, float width, float height, bool flipTextureVertically){ return createRectangle(x, y, width, height, glm::vec4(1.f), flipTextureVertically); }

            static Sprite createRectangle(float x, float y, float width, float height, glm::vec4 color, bool flipTextureVertically){
                Sprite sprite;

                sprite.vertices.initialize(4);
                sprite.colors.initialize(4);
                sprite.texture_coordinates.initialize(4);

                sprite.vertices.add(glm::vec2(x, y));
                sprite.vertices.add(glm::vec2(x + width, y + height));
                sprite.vertices.add(glm::vec2(x + width, y));
                sprite.vertices.add(glm::vec2(x, y + height));

                if(!flipTextureVertically){
                    sprite.texture_coordinates.add(glm::vec2(0.0f, 0.0f));
                    sprite.texture_coordinates.add(glm::vec2(1.0f, 1.0f));
                    sprite.texture_coordinates.add(glm::vec2(1.0f, 0.0f));
                    sprite.texture_coordinates.add(glm::vec2(0.0f, 1.0f));
                }else{
                    sprite.texture_coordinates.add(glm::vec2(0.0f, 1.0f));
                    sprite.texture_coordinates.add(glm::vec2(1.0f, 0.0f));
                    sprite.texture_coordinates.add(glm::vec2(1.0f, 1.0f));
                    sprite.texture_coordinates.add(glm::vec2(0.0f, 0.0f));
                }

                for(int i = 0; i < 4; i++) sprite.colors.add(color);
                sprite.indices = {0, 1, 2, 0, 1, 3};
                sprite.initialize(true);
                
                return(sprite);
            }
    };
}



#endif