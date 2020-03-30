#ifndef VERTEX_ARRAY_BU
#define VERTEX_ARRAY_BU

#include "ToolsGL.hpp"

namespace ulm{

    class VertexArray{
        private:
        public:
            bool initializedBuffers = false;
            unsigned int vertexBuffer = 0;
            unsigned int indexBuffer = 0;

            int numberOfIndices = 0;

            Map<int, Type> attributes;
            
            VertexArray(){}

            void setBuffers(Array<unsigned int>& IB, ByteArray& VB, Map<int, Type>& attributes){
                if(!initializedBuffers)
                    initializeBuffers();
                
                setVertexBuffer(VB, attributes);
                setIndexBuffer(IB);
            }

            void setVertexBuffer(ByteArray& vertices, Map<int, Type>& attribs){
                if(!initializedBuffers)
                    initializeBuffers();

                glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, vertices.size, /*(void*)*/vertices.getPtr(), GL_STATIC_DRAW);

                attributes = attribs;
                setVertexAttrib();
            }

            void setVertexAttrib(){
                int rozdil = 0;

                for(Couple<int, Type> attrib : attributes)
                    rozdil += attrib.x * ulm::Types::sizeOf(attrib.y);

                size_t offset = 0;

                for(int i = 0; i < attributes.size; i++){
                    if(attributes[i].y == INT || attributes[i].y == UINT){
                        glVertexAttribIPointer(i, attributes[i].x, ulm::Types::toGL(attributes[i].y), rozdil, (void*)(offset));
                    }else
                        glVertexAttribPointer(i, attributes[i].x, ulm::Types::toGL(attributes[i].y), GL_FALSE, rozdil, (void*)(offset));

                    glEnableVertexAttribArray(i);

                    offset += attributes[i].x * ulm::Types::sizeOf(attributes[i].y);

                    /*printf("\n i:%d ; s:%d; type:%d", i, attributes[i].x, ulm::Types::toGL(attributes[i].y));
                    if(ulm::Types::toGL(attributes[i].y) == GL_FLOAT)
                        printf("::GL_FLOAT");
                    if(ulm::Types::toGL(attributes[i].y) == GL_INT)
                        printf("::GL_INT");//*/
                }

            }

            void bindAttribs(){
                setVertexAttrib();
            }

            void setIndexBuffer(Array<unsigned int> indices){
                if(!initializedBuffers)
                    initializeBuffers();
                    
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size * sizeof(unsigned int), indices.getPtr(), GL_STATIC_DRAW);

                numberOfIndices = indices.size;
            }


            void draw(){
                bind();
                glDrawElements(GL_TRIANGLES, numberOfIndices, GL_UNSIGNED_INT, 0);
            }

            void drawInstanced(unsigned int count){
                bind();
                glDrawElementsInstanced(GL_TRIANGLES, numberOfIndices, GL_UNSIGNED_INT, 0, count);
            }

            void draw(unsigned int offset, unsigned int size){
                bind();
                glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, (void*)(offset * sizeof(unsigned int)));
            }

            void drawInstanced(unsigned int offset, unsigned int size, unsigned int count){
                bind();
                glDrawElementsInstanced(GL_TRIANGLES, size, GL_UNSIGNED_INT, (void*)(offset * sizeof(unsigned int)), count);
            }

            void bind(){
                glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
                bindAttribs();
            }


            void end(){
                glDeleteBuffers(1, &vertexBuffer);
                glDeleteBuffers(1, &indexBuffer);

                attributes.empty();
                numberOfIndices = 0;
            }

            void initializeBuffers(){
                if(initializedBuffers)
                    end();
                    
                glGenBuffers(1, &vertexBuffer);
                glGenBuffers(1, &indexBuffer);
                initializedBuffers = true;
                
            }

            void copy(const VertexArray& other){
                initializedBuffers = other.initializedBuffers;
                vertexBuffer = other.vertexBuffer;
                indexBuffer = other.indexBuffer;

                numberOfIndices = other.numberOfIndices;
                attributes = other.attributes;
            }

            VertexArray(const VertexArray& other){ copy(other); }
            VertexArray& operator=(const VertexArray& other){ copy(other); return(*this); }
    };


}
#endif