#include "GLM_Base.hpp"

namespace my_glm{

    glm::vec3 rotateVec3(const glm::vec3& vector, float angle, const glm::vec3& axis){
        return(vector * cos(angle) + glm::cross(axis, vector) * sin(angle) + axis*(axis*vector) * (1 - cos(angle)));
    }

}