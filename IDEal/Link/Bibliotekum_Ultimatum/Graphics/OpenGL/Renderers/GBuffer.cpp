#ifndef BU_3D_RENDERER
#define BU_3D_RENDERER

#include "../Tools/ToolsGL.hpp"

namespace ulm{

    struct GBuffer{
        FrameBuffer frameBuffer;
        FrameBuffer outputBuffer;
        int width, height;
        void initialize(int w, int h){ 
            frameBuffer.initializeGBuffer(w, h);  
            outputBuffer.initializeOutputBuffer(w, h, frameBuffer.depthStencilTexture);
            width = w;
            height = h;
        }
        void initialize(glm::ivec2 size){ initialize(size.x, size.y); }

        Texture getResult()             { return(outputBuffer.colorTextures[0]);   }

        Texture getDepthTexture()       { return(frameBuffer.depthStencilTexture); }
        Texture getColorTexture()       { return(frameBuffer.colorTextures[0]);    }      
        Texture getAlbedoTexture()      { return(frameBuffer.colorTextures[1]);    }
        Texture getNormalVecTexture()   { return(frameBuffer.colorTextures[2]);    }

        void bind()     { frameBuffer.bind();    }
        void unbind()   { frameBuffer.unbind();  }

        void clear()    { clear(glm::vec3(0.f)); }

        void clear(glm::vec3 color) {
            outputBuffer.bind();
            outputBuffer.clear(glm::vec3(0.0f));
            frameBuffer.bind(); 
            frameBuffer.clear(color, 1.0, 0xFF); 

            ulm::DefaultFrameBuffer::bind();
        }
    };
}

#endif