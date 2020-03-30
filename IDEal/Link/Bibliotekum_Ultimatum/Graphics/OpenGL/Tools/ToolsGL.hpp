#ifndef ToolsGL_BU_HPP
#define ToolsGL_BU_HPP

namespace ulm{
    enum AccessFreq { STATIC, DYNAMIC };
}

/* Dependancy */

#include "../../../Math/GLM/GLM.hpp"
#include "../../../Base/base.hpp"
#include "../../../Tools/Tools.hpp"
#include "../../Core/Core.hpp"
#include "../../Image/Image.cpp"
#include "../Base/OpenGL.h"

/* Inclusion */

#include "functions.c"
#include "Textures.cpp"
#include "Shader.cpp"
#include "FrameBuffer.cpp"
#include "VertexArray.cpp"
#include "GLSL_library.cpp"
#include "Light.cpp"
#include "ModelMatrixBuffer.cpp"
#include "Skeleton.cpp"
#include "Mesh.cpp"
#include "Model.cpp"
#include "Sprite.cpp"
#include "Billboard.cpp"
#include "SkyBox.cpp"

#endif