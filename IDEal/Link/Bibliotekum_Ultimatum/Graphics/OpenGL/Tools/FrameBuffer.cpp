#ifndef BU_FRAMEBUFFER_TEXTURE
#define BU_FRAMEBUFFER_TEXTURE

#include "ToolsGL.hpp"

namespace ulm{

    //enum FB_type{ FB_null = 0, FB_custom = 1, FB_full = 1, FB_depth = 2 };

    class DefaultFrameBuffer{
        public:
            static int id;
            static int x;
            static int y;
            static int w;
            static int h;

            static void bind(){
                glBindFramebuffer(GL_FRAMEBUFFER, id); 
                glViewport(x, y, w, h);
            }
    };

    int DefaultFrameBuffer::id = 0;
    int DefaultFrameBuffer::x = 0;
    int DefaultFrameBuffer::y = 0;
    int DefaultFrameBuffer::w = 0;
    int DefaultFrameBuffer::h = 0;

    class FrameBuffer{
        private:
            //FB_type typ = FB_null;

        public:
            bool initialized = false;
            unsigned int id = 0;

            Texture depthStencilTexture;
            Array<Texture> colorTextures;

            unsigned int depthStencilRenderBuffer = 0;
            int width = 0;
            int height = 0;
            int nr_channels = 0;

            bool depth_buffer = false;
            bool stencil_buffer = false;
            bool color_buffer = false;

            void initializeColored(int w, int h, int channels, bool depth_test, bool stencil_test){
                if(initialized) end();
                else initialized = true;

                glGenFramebuffers(1, &id);
                glBindFramebuffer(GL_FRAMEBUFFER, id); 

                width = w;
                height = h;
                nr_channels = channels; 
                color_buffer = true;    
                depth_buffer = depth_test;
                stencil_buffer = stencil_test;    

                createColorTexture(w, h, channels, 0);

                if(depth_test || stencil_test){
                    unsigned int RbId;
                    glGenRenderbuffers(1, &RbId);
                    glBindRenderbuffer(GL_RENDERBUFFER, RbId); 
                    if(depth_test && stencil_test)  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);  
                    else if(depth_test)             glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h); 
                    else if(stencil_test)           glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, w, h);
                    glBindRenderbuffer(GL_RENDERBUFFER, 0);
                    if(depth_test && stencil_test)  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RbId);
                    else if(depth_test)             glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RbId);
                    else if(stencil_test)           glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RbId);
                    
                    depthStencilRenderBuffer = RbId;
                }

                printStatus();
                    
                unbind();  
            }

            void initializeDepth(int w, int h){
                if(initialized) end();
                else initialized = true;

                width = w;
                height = h;
                nr_channels = 0; 
                color_buffer = false;    
                depth_buffer = true;
                stencil_buffer = false;

                glGenFramebuffers(1, &id);  
                glBindFramebuffer(GL_FRAMEBUFFER, id);

                createDepthTexture(w, h);

                #ifndef BU_MOBILE
                    glDrawBuffer(GL_NONE);
                #endif
                glReadBuffer(GL_NONE);

                printStatus();
                
                unbind();
            }

            void initializeOutputBuffer(int w, int h, Texture depthStencilTex){
                if(initialized) end();
                else initialized = true;

                depth_buffer = false;
                stencil_buffer = false;
                color_buffer = true;

                width = w;
                height = h;
                nr_channels = 3;

                glGenFramebuffers(1, &id);  
                glBindFramebuffer(GL_FRAMEBUFFER, id);

                createColorTexture(w, h, 3, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencilTex.id, 0);
                depthStencilTexture = depthStencilTex;
                printStatus();
                unbind();
            }

            void initializeGBuffer(int w, int h){
                if(initialized) end();
                else initialized = true;

                depth_buffer = true;
                stencil_buffer = true;
                color_buffer = true;

                width = w;
                height = h;
                nr_channels = 3;

                glGenFramebuffers(1, &id);  
                glBindFramebuffer(GL_FRAMEBUFFER, id);
                
                createDepthStencilTexture(w, h);

                createColorTexture(w, h, 3, 0);
                createColorTexture(w, h, 3, 1);
                createColorTexture(w, h, 3, 2);
                
                unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
                glDrawBuffers(3, attachments);
                
                printStatus();

                unbind();
            }

            void create_DepthStencilRenderBuffer(int w, int h){
                unsigned int RbId;
                glGenRenderbuffers(1, &RbId);
                glBindRenderbuffer(GL_RENDERBUFFER, RbId); 
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);  
                
                glBindRenderbuffer(GL_RENDERBUFFER, 0);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RbId);
                depthStencilRenderBuffer = RbId;                    
            }

            void setRenderBuffer(bool depth, bool stencil, unsigned int RbId){

                if(stencil) glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RbId);
                else if(depth)       glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RbId);
                
                depthStencilRenderBuffer = RbId;                    
            }

            void createColorTexture(int w, int h, int channels, int attachment){
                Texture texture;
                texture.originalSize = glm::vec2(w, h);
                texture.initialized = true;
                texture.numberOfChannels = channels;

                glGenTextures(1, &texture.id);
                TextureManager::use(texture);

                #ifdef BU_MOBILE
                    if(channels == 4) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,             w, h,  0, GL_RGBA,            GL_UNSIGNED_BYTE, NULL);
                    if(channels == 3) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,              w, h,  0, GL_RGB,             GL_UNSIGNED_BYTE, NULL);
                    if(channels == 2) glTexImage2D(GL_TEXTURE_2D, 0, GL_RG,               w, h,  0, GL_RG,              GL_UNSIGNED_BYTE, NULL);
                    if(channels == 1) glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,              w, h,  0, GL_RED,             GL_UNSIGNED_BYTE, NULL);
                #else
                    if(channels == 4) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,            w, h,  0, GL_RGBA,            GL_UNSIGNED_BYTE, NULL);
                    if(channels == 3) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8,             w, h,  0, GL_RGB,             GL_UNSIGNED_BYTE, NULL);
                    if(channels == 2) glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8,              w, h,  0, GL_RG,              GL_UNSIGNED_BYTE, NULL);
                    if(channels == 1) glTexImage2D(GL_TEXTURE_2D, 0, GL_R8,               w, h,  0, GL_RED,             GL_UNSIGNED_BYTE, NULL);
                #endif
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment, GL_TEXTURE_2D, texture.id, 0);

                colorTextures.add(texture);
            }

            void createDepthTexture(int w, int h){
                depthStencilTexture.initialized = true;
                glGenTextures(1, &depthStencilTexture.id);
                TextureManager::use(depthStencilTexture);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, w, h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthStencilTexture.id, 0);
            }

            void createDepthStencilTexture(int w, int h){
                depthStencilTexture.initialized = true;
                glGenTextures(1, &depthStencilTexture.id);
                TextureManager::use(depthStencilTexture);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, w, h, 0,  GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencilTexture.id, 0);
            
            }

            void bind(){
                glBindFramebuffer(GL_FRAMEBUFFER, id);
                glViewport(0, 0, width, height);
            }

            void clear(glm::vec3 color, float depthVal, int stencilMask){
                glStencilMask(stencilMask);
                clear(color, depthVal);
            }


            void clear(glm::vec3 color, float depthVal){
                glDepthMask(GL_TRUE);
                #ifdef BU_MOBILE
                    glClearDepthf(depthVal);
                #else
                    glClearDepth(depthVal);
                #endif
                clear(color);
            }

            void clear(glm::vec3 color){
                glClearColor(color.x, color.y, color.z, 1.0f);
                clear();
            }

            void clear(){
                int buffers = 0;
                if(color_buffer)    buffers |= GL_COLOR_BUFFER_BIT;
                if(depth_buffer)    buffers |= GL_DEPTH_BUFFER_BIT;
                if(stencil_buffer)  buffers |= GL_STENCIL_BUFFER_BIT;

                glClear(buffers);
            }

            void unbind(){
                DefaultFrameBuffer::bind();
            }

            void printStatus(){
                int status = getStatus();
                if(status != GL_FRAMEBUFFER_COMPLETE){
                    char mess[512];
                    sprintf(mess, "%X/%d", status, status);
                    String error = String("ULM::ERROR::FRAMEBUFFER::Initialization::") + mess;
                    if(status == GL_FRAMEBUFFER_UNSUPPORTED)
                        error += "::GL_FRAMEBUFFER_UNSUPPORTED";
                    if(status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
                        error += "::GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
                    Properties::handleError(error);
                }
            }

            int getStatus(){
                return((int)glCheckFramebufferStatus(GL_FRAMEBUFFER));
            }

            void end(){
                glDeleteFramebuffers(1, &id);

                depthStencilTexture.end();
                for(Texture& tex : colorTextures) tex.end();
                colorTextures.empty();

                //glDeleteRenderbuffers(RbIds.size, RbIds.getPtr());
                if(depthStencilRenderBuffer != 0) glDeleteRenderbuffers(1, &depthStencilRenderBuffer);
                depthStencilRenderBuffer = 0;

                depth_buffer = false;
                stencil_buffer = false;
                color_buffer = false;
            }
    };

}

#endif