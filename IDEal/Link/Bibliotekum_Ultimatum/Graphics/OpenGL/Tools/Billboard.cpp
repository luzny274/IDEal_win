#ifndef BILLBOARD_BU
#define BILLBOARD_BU

#include "ToolsGL.hpp"

namespace ulm{

    class PositionBuffer{
        private:
            unsigned int bufferID = 0;

        public:
            unsigned int size = 0;
            bool initialized = false;

            void initialize(int arg_size){
                initialize(arg_size, STATIC);
            }

            void initialize(int arg_size, AccessFreq accessFreq){
                if(initialized) end();
                else initialized = true;

                size = (unsigned int)arg_size;

                glGenBuffers(1, &bufferID);
                glBindBuffer(GL_ARRAY_BUFFER, bufferID);

                glm::vec4 * pole = new glm::vec4[arg_size];
                for(int i = 0; i < arg_size; i++)
                    pole[i] = glm::vec4(1.f);

                if(accessFreq == STATIC)
                    glBufferData(GL_ARRAY_BUFFER, arg_size * sizeof(glm::vec4), pole, GL_STATIC_DRAW);
                else if(accessFreq == DYNAMIC)
                    glBufferData(GL_ARRAY_BUFFER, arg_size * sizeof(glm::vec4), pole, GL_DYNAMIC_DRAW);                

                delete[] pole;
            }

            void bind(){
                glBindBuffer(GL_ARRAY_BUFFER, bufferID);
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
                glVertexAttribDivisor(1, 1);
            }

            void push(int startIndex, int arg_size, glm::vec4 * positions){
                if(startIndex + arg_size > (int)size)
                    Properties::handleError("ULM::ERROR::PositionBuffer::push::parameters exceed size of buffer");
                glBindBuffer(GL_ARRAY_BUFFER, bufferID);
                glBufferSubData(GL_ARRAY_BUFFER, startIndex * sizeof(glm::vec4), arg_size * sizeof(glm::vec4), (void*) positions);
            }

            void push(int startIndex, int arg_size, Array<glm::vec4> positions){
                if(arg_size > positions.size)
                    Properties::handleError("ULM::ERROR::PositionBuffer::push::argument size is bigger than size of glm::vec4 array");
                push(startIndex, arg_size, positions.getPtr());
            }

            void end(){
                if(initialized) glDeleteBuffers(1, &bufferID);
            }

    };

    class Billboard{
        public:
            bool initialized = false;

            float width = 0.f;
            float height = 0.f;
            glm::vec4 position = glm::vec4(0.f); // x, y, z - world position, w - lifeTime - value from 0.f to 1.f
            PositionBuffer positionBuffer;
            
            glm::vec4 multiplier = glm::vec4(1.f);

            unsigned int textureAtlasLength = 1; // expects square root of number of phases inside texture atlas
            VertexArray va;

            Texture textureAtlas; // expects texture atlas containing square array of phases, in order from left to right, bottom to up

            void initialize(){
                if(initialized) end();
                else initialized = true;

                ByteArray vb = ulm::ByteArray::parse({  0.f, 0.f,
                                                        0.f, 1.f,
                                                        1.f, 0.f,
                                                        1.f, 1.f});
                Map<int, Type> attr = Map<int, Type>({2}, FLOAT);

                Array<unsigned int> indices = {0, 3, 1, 0, 3, 2};

                va.setBuffers(indices, vb, attr);
            }

            void end(){
                if(!initialized) va.end();
                width = 0.f;
                height = 0.f;
                position = glm::vec4(0.f);
                initialized = false;
            }
    };
}

#endif