#ifndef BU_CARDBOARD_VR_CLASS
#define BU_CARDBOARD_VR_CLASS

#include "../Core.hpp"

namespace ulm{
    class CardboardVR{
        public:
            static glm::vec3 front;
            static glm::vec3 up;
            static glm::vec3 right;
            static glm::mat4 perspective;

            static float znear;
            static float zfar;

            static glm::vec3 getFront(){ return front; }
            static glm::vec3 getUp(){ return up; }
            static glm::vec3 getRight(){ return right; }
            static glm::mat4 getPerspective(){ return perspective; }

            static void setNearPlane(float val) { znear = val; }
            static void setFarPlane(float val) { zfar = val; }
    };

    glm::vec3   CardboardVR::front = glm::vec3(0.f);
    glm::vec3   CardboardVR::up = glm::vec3(0.f);
    glm::vec3   CardboardVR::right = glm::vec3(0.f);
    glm::mat4   CardboardVR::perspective = glm::mat4(1.f);

    float       CardboardVR::znear = 0.1f;
    float       CardboardVR::zfar = 100.0f;
}

#endif