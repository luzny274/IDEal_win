#ifndef BU_GLM_TRANSFORM
#define BU_GLM_TRANSFORM

namespace ulm{

    enum TriangleOrientation{ Coliniar = 0, Clockwise = 1, CounterClockwise = 2 };

    class Math{
        public:
            static glm::vec3 rotateVec3(glm::vec3 vector, float angle, glm::vec3 axis){
                return(vector * cos(angle) + glm::cross(axis, vector) * sin(angle) + axis*(axis*vector) * (1 - cos(angle)));
            }

            static glm::mat4 rotate2D(glm::mat4 matrix, float angle){
                return glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));
            }

            static void checkMatrix(const char * mess, glm::mat4 mat){
                bool error = false;
                for(int i = 0; i < 4; i++)
                    for(int ii = 0; ii < 4; ii++)
                        if(isnan(mat[i][ii]) || isinf(mat[i][ii]))
                            error = true;

                if(error) Properties::handleError(mess);
            }

            static void printMat(glm::mat4 arg){
                printf("\n");
                for(int i = 0; i < 4; i++)
                    printf("\n%f  %f  %f  %f", arg[i][0], arg[i][1], arg[i][2], arg[i][3]);
            }

            static void printMat(glm::mat3 arg){
                printf("\n");
                for(int i = 0; i < 3; i++)
                    printf("\n%f  %f  %f", arg[i][0], arg[i][1], arg[i][2]);
            }

            static void printVec(glm::vec2 vec) { printf("\n %f  %f\n", vec.x, vec.y); }
            static void printVec(glm::vec3 vec) { printf("\n %f  %f  %f\n", vec.x, vec.y, vec.z); }
            static void printVec(glm::vec4 vec) { printf("\n %f  %f  %f  %f\n", vec.x, vec.y, vec.z, vec.w); }

            static void printVec(glm::ivec2 vec){ printf("\n %d  %d\n", vec.x, vec.y); }
            static void printVec(glm::ivec3 vec){ printf("\n %d  %d  %d\n", vec.x, vec.y, vec.z); }
            static void printVec(glm::ivec4 vec){ printf("\n %d  %d  %d  %d\n", vec.x, vec.y, vec.z, vec.w); }

            static glm::vec3 getAnyPerpendicularVec(glm::vec3 vec){
                float thresh = 0.1f;
                vec = glm::normalize(vec);

                if((abs(vec.x) < thresh && abs(vec.y) > (1.0f - thresh) && abs(vec.z) < thresh)) 
                    return glm::cross(vec, glm::vec3(0.0f, 0.0f, 1.0f));
                else 
                    return glm::cross(vec, glm::vec3(0.0f, 1.0f, 0.0f));
            }

            static float getAngle(glm::vec3 vec1, glm::vec3 vec2){
                float val = (glm::dot(vec1, vec2))/(glm::length(vec1) * glm::length(vec2));
                return(glm::acos(val));
            }

            static float getAngle(glm::vec3 vec1, glm::vec3 vec2, glm::vec3 origin){
                vec1 -= origin;
                vec2 -= origin;
                return(getAngle(vec1, vec2));
            }

            static TriangleOrientation getTriangleOrientation(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 normal){
                float val = glm::dot(normal, glm::cross((p2-p1),(p3-p1)));
                if (val == 0) return Coliniar;
                return (val > 0)? CounterClockwise : Clockwise;
            }
            
            static bool matchTriangles(glm::vec3 * triangle1, glm::vec3 * triangle2){
                int pom = 0;

                for(int i = 0; i < 3; i++)
                    for(int ii = 0; ii < 3; ii++)
                        if(triangle1[i] == triangle2[ii])
                            pom++;

                if(pom >= 3)
                    return true;
                else
                    return false;
            }

            template<typename typ> static int sgn(typ val){
                return (typ(0) < val) - (val < typ(0));
            }

            template<typename typ> static void swap(typ * p1, typ * p2){
                typ pom = *p1;
                *p1 = *p2;
                *p2 = pom;
            }

            template<typename typ> static typ max(typ p1, typ p2){
                if(p1 >= p2)
                    return p1;
                else
                    return p2;
            }

            template<typename typ> static typ min(typ p1, typ p2){
                if(p1 <= p2)
                    return p1;
                else
                    return p2;
            }

            static bool checkFlag(std::uint_fast8_t flags, std::uint_fast8_t flag){
                return((flags & flag) > 0);
            }
    };
}

#endif