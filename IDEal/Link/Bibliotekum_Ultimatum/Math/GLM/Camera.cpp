#ifndef ULM_GLM_CAMERA
#define ULM_GLM_CAMERA

#include "GLM.hpp"

namespace ulm{
    class Camera{
        public:
            glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);

            glm::mat4 view;
            glm::mat4 projection = glm::mat4(1.0f);  

            glm::vec3 front  = glm::vec3(0.0f, 0.0f,  1.0f);
            glm::vec3 up     = glm::vec3(0.0f, 1.0f,  0.0f);

            float limitAngle = 0.1f;

            Camera(){};

            glm::mat4 getProjection(){ return projection; }

            glm::mat4 getView(){
                view = glm::lookAt(position, position + front, up);
                Math::checkMatrix("ULM::WARNING::Camera::getView::NaN inside matrix", view);
                return (view);
            }

            glm::mat4 getVP(){
                view = getView();
                glm::mat4 VP = projection * view;
                Math::checkMatrix("ULM::WARNING::Camera::getVP::NaN inside matrix", view);
                return (VP);
            }
            
            glm::mat4 getVPWithoutTrans(){
                view = glm::mat4(glm::mat3(getView()));
                glm::mat4 VP = projection * view;
                Math::checkMatrix("ULM::WARNING::Camera::getVP::NaN inside matrix", view);
                return (VP);
            }

            glm::vec3 getRight(){
                view = getView();
                return {view[0][0], view[1][0], view[2][0]};
            }

            glm::vec3 getUp(){
                view = getView();
                return {view[0][1], view[1][1], view[2][1]};
            }


            void createPerspective(float FOV, float ratio, float nearPlane, float farPlane){
                projection = glm::perspective(FOV, ratio, nearPlane, farPlane);
                Math::checkMatrix("ULM::WARNING::Camera::createPerspective::NaN inside matrix", projection);
            }

            void createOrthographic(
                float left, 
                float right, 
                float bottom, 
                float top, 
                float nearPlane, 
                float farPlane)
            {
                projection = glm::ortho(left, right, bottom, top, nearPlane, farPlane); 
                Math::checkMatrix("ULM::WARNING::Camera::createOrthographic::NaN inside matrix", projection);
            }

            void yaw(float angle){
                front = glm::normalize(my_glm::rotateVec3(front, angle, up));
            }

            void pitch(float angle){
                glm::vec3 newFront = glm::normalize(my_glm::rotateVec3(front, angle, glm::normalize(glm::cross(front, up))));
                if(Math::getAngle(newFront, up) > limitAngle)
                    front = newFront;
            }    

            void forward(float value){ position += value * front; }
            void right(float value) { position += glm::normalize(glm::cross(front, up)) * value; }
            void higher(float value) { position += up * value; }
            
            void move(float arg_right, float arg_forth, float arg_up){
                if(arg_right != 0.0f) right(arg_right);
                if(arg_forth != 0.0f) forward(arg_forth);
                if(arg_up != 0.0f) higher(arg_up);
            }

    };

    class CameraService{
        public:

            static void setPerspective(Camera * camera, float FOV, float ratio, float nearPlane, float farPlane){
                camera->projection = glm::perspective(FOV, ratio, nearPlane, farPlane);
                Math::checkMatrix("ULM::WARNING::CameraService::setPerspective::NaN inside matrix", camera->projection);
            }

            static void setOrthographic(Camera * camera,
                float left, 
                float right, 
                float bottom, 
                float top, 
                float nearPlane, 
                float farPlane)
            {
                camera->projection = glm::ortho(left, right, bottom, top, nearPlane, farPlane); 
                Math::checkMatrix("ULM::WARNING::CameraService::setOrthographic::NaN inside matrix", camera->projection);
            }

            static void yaw(Camera * camera, float angle){
                camera->front = glm::normalize(my_glm::rotateVec3(camera->front, angle, camera->up));
            }

            static void pitch(Camera * camera, float angle){

                camera->front = glm::normalize(my_glm::rotateVec3(camera->front, angle, glm::normalize(glm::cross(camera->front, camera->up))));
                
            }         

            static void roll(Camera * camera, float angle){
                glm::vec3 axis = glm::normalize(glm::cross(camera->up, glm::cross(camera->front, camera->up)));
                camera->up = my_glm::rotateVec3(camera->up, angle, axis);;
            }

            static void move(Camera * camera, float right, float forth, float up){
                if(right != 0.0f) camera->position += glm::normalize(glm::cross(camera->front, camera->up)) * right;
                if(forth != 0.0f) camera->position += forth * camera->front;
                if(up != 0.0f) camera->position += camera->up * up;
            }
            static void translate(Camera * camera, float dX, float dY, float dZ){
                if(dX != 0.0f) camera->position.x += dX;
                if(dY != 0.0f) camera->position.y += dY;
                if(dZ != 0.0f) camera->position.z += dZ;
            }
            static void setPosition(Camera * camera, float x, float y, float z){
                camera->position.x = x;
                camera->position.y = y;
                camera->position.z = z;
            }


            static float pitchLimit;

            static void yawNpitch(Camera * camera, float dAngleX, float dAngleY){
                /*camera->yawed   += dAngleX;
                camera->pitched -= dAngleY;

                if(camera->pitched > glm::radians(89.0f))
                    camera->pitched = glm::radians(89.0f);
                if(camera->pitched < -glm::radians(89.0f))
                    camera->pitched= -glm::radians(89.0f);

                camera->front.x = cos(camera->yawed) * cos(camera->pitched);
                camera->front.y = sin(camera->pitched);
                camera->front.z = sin(camera->yawed) * cos(camera->pitched);

                camera->front = glm::normalize(camera->front);//*/
            }

/*
            static struct FPSLIKE{
                static struct Rotation{
                    static float pitchLimit = 0.1f;

                    void yawNpitch(Camera * camera, float dAngleX, float dAngleY){

                        float fuAngle = glm::angle(glm::normalize(camera->front), glm::vec3(0.0f, 1.0f,  0.0f));
                        if(fuAngle < pitchLimit){
                            camera->front = my_glm::rotateVec3(camera->front, pitchLimit - fuAngle, glm::normalize(glm::cross(camera->front, glm::vec3(0.0f, 1.0f,  0.0f))));

                        }else camera->front = my_glm::rotateVec3(camera->front, dAngleY, glm::normalize(glm::cross(camera->front, glm::vec3(0.0f, 1.0f,  0.0f))));
                        
                        camera->front = my_glm::rotateVec3(camera->front, dAngleX, glm::vec3(0.0f, 1.0f,  0.0f));

                        
                        
                    }
                    void roll(Camera * camera, float angle){
                        glm::vec3 axis = glm::normalize(glm::cross(camera->up, glm::cross(camera->front, camera->up)));
                        camera->up = my_glm::rotateVec3(camera->up, angle, axis);;
                    }
                }Rotate;

                static struct Movement{
                    void move(Camera * camera, float right, float forth, float up){
                        if(right != 0.0f) camera->position += glm::normalize(glm::cross(camera->front, camera->up)) * right;
                        if(forth != 0.0f) camera->position += forth * camera->front;
                        if(up != 0.0f) camera->position += camera->up * up;
                    }
                }Move;
            }FPSlike;//*/

    };

    float CameraService::pitchLimit = 0.1f;

}

#endif