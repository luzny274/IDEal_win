#ifndef BU_MODEL_MATRIX_BUFFER
#define BU_MODEL_MATRIX_BUFFER

#include "ToolsGL.hpp"

namespace ulm{

    class ModelMatrixBuffer{
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

                glm::mat4 * pole = new glm::mat4[arg_size];
                for(int i = 0; i < arg_size; i++)
                    pole[i] = glm::mat4(1.f);

                if(accessFreq == STATIC)
                    glBufferData(GL_ARRAY_BUFFER, arg_size * sizeof(glm::mat4), pole, GL_STATIC_DRAW);
                else if(accessFreq == DYNAMIC)
                    glBufferData(GL_ARRAY_BUFFER, arg_size * sizeof(glm::mat4), pole, GL_DYNAMIC_DRAW);                
                
                delete[] pole;
            }

            void bind(){
                glBindBuffer(GL_ARRAY_BUFFER, bufferID);
                glEnableVertexAttribArray(8);
                glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
                glEnableVertexAttribArray(9);
                glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
                glEnableVertexAttribArray(10);
                glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
                glEnableVertexAttribArray(11);
                glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
                
                glVertexAttribDivisor(8, 1);
                glVertexAttribDivisor(9, 1);
                glVertexAttribDivisor(10, 1);
                glVertexAttribDivisor(11, 1);
            }

            void push(int startIndex, int arg_size, glm::mat4 * matrices){
                if(startIndex + arg_size > (int)size)
                    Properties::handleError("ULM::ERROR::ModelMatrixBuffer::push::parameters exceed size of buffer");
                glBindBuffer(GL_ARRAY_BUFFER, bufferID);
                glBufferSubData(GL_ARRAY_BUFFER, startIndex * sizeof(glm::mat4), arg_size * sizeof(glm::mat4), (void*) matrices);
            }
            void push(int startIndex, int arg_size, Array<glm::mat4> matrices){
                if(arg_size > matrices.size)
                    Properties::handleError("ULM::ERROR::ModelMatrixBuffer::push::argument size is bigger than size of glm::mat4 array");
                push(startIndex, arg_size, matrices.getPtr());
            }

            void end(){
                if(initialized) glDeleteBuffers(1, &bufferID);
            }

    };
}


#endif