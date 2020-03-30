#ifndef GLAD_INCLUDE_BU
#define GLAD_INCLUDE_BU

#ifdef BU_MOBILE
    #ifdef BU_VR
        #include <GLES3/gl3.h>
        //#include "Mobile/glad_gles.h"
        //#include "Mobile/glad_gles.c"
    #else
        #include "Mobile/glad_gles.h"
        #include "Mobile/glad_gles.c"
    #endif
#else
    #include "PC/glad_gl.h"
    #include "PC/glad_gl.c"
#endif

#ifdef BU_GLF
    #include "PC/glfw3.h"
#endif



#endif