#ifndef BU_PROGRAM_STRUCTURE
#define BU_PROGRAM_STRUCTURE

#include "../../Base/base.hpp"
#include "../../Tools/Tools.hpp"

namespace ulm{

    enum Eye {leftEye = -1, nullEye = 0, rightEye = 1};
    
    class Program{
        public:
            Program(){};
            virtual void update(float deltaTime) = 0;
            virtual void render(Eye eye) = 0;
            virtual void resizeCallback(int width, int height) = 0;
            virtual ~Program(){};
    };
    
}

#endif 