#ifndef BU_SKELETON_BUFFER
#define BU_SKELETON_BUFFER

#include "ToolsGL.hpp"

namespace ulm{
    class Joint{
        public:
            String name;
            int parent;

            glm::mat4 inverseDefault = glm::mat4(1.f);
            glm::vec3 translation;
            glm::quat rotation;
            glm::vec3 scale;

            glm::mat4 getMatrix(){
                glm::mat4 mat(1.f);

                mat = glm::translate (mat, translation);
                mat = glm::scale     (mat, scale);

                return(mat * glm::toMat4(rotation));

            }
    };

    class Skeleton{

        public:
            Array<Joint> joints;

            void interpolate(Skeleton& other, float coef){
                if(joints.size != other.joints.size)
                    ulm::Properties::handleError("ULM::ERROR::SKELETON::interpolate::key frames have different sizes");
                if(coef > 1.f || coef < 0.f)
                    ulm::Properties::handleError("ULM::ERROR::SKELETON::interpolate::argument coef has to be less than 1 and greater than 0");                
                
                for(int i = 0; i < Math::min(other.joints.size, joints.size); i++){
                    joints[i].translation = coef * joints[i].translation + (1.f - coef) * other.joints[i].translation;
                    joints[i].scale = coef * joints[i].scale + (1.f - coef) * other.joints[i].scale;
                    joints[i].rotation = glm::slerp(joints[i].rotation, other.joints[i].rotation, coef);
                }
            } 

            static Skeleton interpolation(Skeleton& first, Skeleton& second, float coef){
                Skeleton novy = first;
                novy.interpolate(second, coef);
                return novy;
            }

            Array<glm::mat4> toMatrices(){
                Array<glm::mat4> matrices;

                for(int i = 0; i < joints.size; i++){
                    glm::mat4 matrix(1.f);

                    int index = i;
                    int cnt = 0;
                    while(index >= 0)
                    {
                        matrix = joints[index].getMatrix() * matrix;
                        index = joints[index].parent;

                        if(cnt > joints.size){
                            Properties::handleError("ULM::ERROR::SKELETON::toMatrices::joints have cyclical inheritance");
                            break;
                        }
                        cnt++;
                    }

                    glm::vec3 b(0.f);
                    index = i;
                    cnt = 0;
                    while(index >= 0)
                    {
                        b -= joints[index].translation;
                        index = joints[index].parent;

                        if(cnt > joints.size){
                            Properties::handleError("ULM::ERROR::SKELETON::toMatrices::joints have cyclical inheritance");
                            break;
                        }
                        cnt++;
                    }
                    matrix = matrix * glm::translate(glm::mat4(1.f), b);

                    Math::checkMatrix("ULM::ERROR::SKELETON::toMatrices::rot contains NaN", matrix);
                    Math::checkMatrix("ULM::ERROR::SKELETON::toMatrices::matrix contains NaN", matrix);


                    matrices.add(matrix * joints[i].inverseDefault);
                }

                return(matrices);
            }

            void subtract(Skeleton& skeleton){
                if(joints.size != skeleton.joints.size)
                    Properties::handleError("ULM::ERROR::SKELETON::subtract::skeleton sizes are not equal");

                for(int i = 0; i < joints.size; i++){
                    joints[i].translation -= skeleton.joints[i].translation;
                    joints[i].rotation = joints[i].rotation * glm::inverse(skeleton.joints[i].rotation);
                    joints[i].scale /= skeleton.joints[i].scale;
                }
            }
    };


    class SkeletonBuffer{
        private:
            unsigned int bufferID = 0;

        public:
            unsigned int size = 0;
            int skeletonSize = 0;
            bool initialized = false;

            void initialize(int arg_size, int skeleton_size){
                initialize(arg_size, skeleton_size, STATIC);
            }

            void initialize(int arg_size, int skeleton_size, AccessFreq accessFreq){
                if(skeleton_size > 0){
                    if(initialized) end();
                    else initialized = true;

                    size = (unsigned int)(arg_size * skeleton_size);
                    skeletonSize = skeleton_size;

                    glGenBuffers(1, &bufferID);
                    glBindBuffer(GL_UNIFORM_BUFFER, bufferID);

                    glm::mat4 * pole = new glm::mat4[size];
                    for(int i = 0; i < (int)size; i++)
                        pole[i] = glm::mat4(1.f);

                    if(accessFreq == STATIC)
                        glBufferData(GL_UNIFORM_BUFFER, size * sizeof(glm::mat4), pole, GL_STATIC_DRAW);
                    else if(accessFreq == DYNAMIC)
                        glBufferData(GL_UNIFORM_BUFFER, size * sizeof(glm::mat4), pole, GL_DYNAMIC_DRAW);                
                
                    delete[] pole;
                }else{
                    Properties::handleError("ULM::ERROR::SkeletonBuffer::intialize::skeleton size must be a positive non-zero integer");
                }
            }

            void bind(){
                if(initialized) glBindBufferRange(GL_UNIFORM_BUFFER, 0, bufferID, 0, size * sizeof(glm::mat4));
            }

            void push(int startIndex, int arg_size, Skeleton * skeletons){
                Array<glm::mat4> matrices(size);

                for(int i = 0; i < arg_size; i++){
                    if(skeletons[i].joints.size != skeletonSize) 
                        Properties::handleError("ULM::ERROR::Skeleton::push::Some skeletons sizes in array are not equal skeleton size specified during buffer initialization");
                    
                    matrices.add(skeletons[i].toMatrices());
                }
                push(startIndex * skeletonSize, arg_size * skeletonSize, matrices.getPtr());
            }

            void push(int startIndex, int arg_size, Array<Skeleton>& skeletons){
                if(arg_size > skeletons.size)
                    Properties::handleError("ULM::ERROR::Skeleton::push::argument size is bigger than size of skeleton array");
                
                push(startIndex, arg_size, skeletons.getPtr());
            }

            void push(int startIndex, int arg_size, glm::mat4 * matrices){
                if(startIndex + arg_size > (int)size)
                    Properties::handleError("ULM::ERROR::Skeleton::push::parameters exceed size of buffer");

                glBindBuffer(GL_UNIFORM_BUFFER, bufferID);
                glBufferSubData(GL_UNIFORM_BUFFER, startIndex * sizeof(glm::mat4), arg_size * sizeof(glm::mat4), (void*) matrices);
            }

            void end(){
                if(initialized) glDeleteBuffers(1, &bufferID);
            }

    };
}


#endif