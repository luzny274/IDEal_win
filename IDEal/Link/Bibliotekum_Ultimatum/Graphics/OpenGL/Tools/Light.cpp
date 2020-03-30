#ifndef BU_LIGHT_CLASS
#define BU_LIGHT_CLASS

#include "ToolsGL.hpp"

namespace ulm{
    class DirectionalLight{
        public:
            glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
            glm::vec3 color = glm::vec3(1.0f);
            float ambient = 0.0f;

            glm::vec3 getDirection(){ return(glm::normalize(direction)); }

            DirectionalLight(){}
            DirectionalLight(glm::vec3 arg_dir, glm::vec3 arg_color, float arg_ambient){
                direction = arg_dir;
                color = arg_color;
                ambient = arg_ambient;
            }
    };

    class PointLight{
        public:
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            glm::vec3 position = glm::vec3(0.0f);
            glm::vec3 color = glm::vec3(1.0f);
            float ambient = 0.0f;

            float range = 0.0f;
            float linear = 0.0f;
            float quadratic = 0.0f;

            PointLight(){}
            PointLight(glm::vec3 arg_position, glm::vec3 arg_color, float arg_ambient, float arg_range){
                position = arg_position; color = arg_color; ambient = arg_ambient;
                setRange(arg_range);
            }

            void setRange(float arg_range){
                range = arg_range;
                linear = 4.5f / range / 3.0f;
                quadratic = 75.0f / (range * range) / 9.0f;
            }
            
            glm::vec3 getPosition(){
                return(glm::vec3(modelMatrix * glm::vec4(position, 1.0f)));
            }

            void copy(const PointLight& other){
                modelMatrix = other.modelMatrix;
                position = other.position;
                color = other.color;
                ambient = other.ambient;
                range = other.range;
                linear = other.linear;
                quadratic = other.quadratic;
            }

            PointLight(const PointLight& other){
                copy(other);
            }     

            PointLight& operator=(const PointLight& other){
                copy(other);
                return(*this);
            }       
    };

    class SpotLight{
        public:
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            glm::vec3 position = glm::vec3(0.0f);  
            glm::vec3 color = glm::vec3(0.0f); 
            glm::vec3 direction = glm::vec3(0.0f);

            float ambient = 0.0f;

            float angle = 0.0f;

            float range = 0.0f;
            float linear = 0.0f;
            float quadratic = 0.0f;


            float nearPlane = 0.1f;
            FrameBuffer shadowMap;
            float shadowQuality = 1.0f;
            float bias = 0.0f;

            SpotLight(){}
            SpotLight(glm::vec3 arg_position, glm::vec3 arg_color, glm::vec3 arg_direction, float arg_ambient, float arg_range, float arg_angle){
                position = arg_position; color = arg_color; direction = arg_direction; angle = arg_angle; ambient = arg_ambient;
                setRange(arg_range);
            }

            void setRange(float arg_range){
                range = arg_range;
                linear = 4.5f / range / 3.0f;
                quadratic = 75.0f / (range * range) / 9.0f;
            }

            glm::vec3 getPosition(){
                return(glm::vec3(modelMatrix * glm::vec4(position, 1.0f)));
            }

            glm::vec3 getDirection(){                
                return(glm::normalize(glm::mat3(glm::transpose(glm::inverse(modelMatrix))) * direction));
            }

            glm::mat4 getVP(){
                glm::vec3 pos = getPosition();
                glm::vec3 dir = getDirection();

                glm::mat4 projection =  glm::perspective(angle * 2.0f, 1.0f, nearPlane, range);
                glm::mat4 view =        glm::lookAt(pos, pos + dir, glm::vec3(0.0, 1.0, 0.0));

                return(projection * view);
            }

            void bindShadowMap(FrameBuffer& fb){
                shadowMap = fb;
            }


    };
}

#endif