#ifndef BU_SKYBOX_RENDERER
#define BU_SKYBOX_RENDERER

#include "../Tools/ToolsGL.hpp"
#include "Renderer3D.cpp"

namespace ulm{

    class SkyBoxShader{
        private:
            static String vertex;
            static String fragment;
            Shader shader;
        public:
            int VP = -1;
            int tex = -1;

            void initialize(){
                shader.initialize(IncludeGLSL::begin + vertex, IncludeGLSL::begin + fragment);
                VP =  glGetUniformLocation(shader.programID, "VP" );
                tex = glGetUniformLocation(shader.programID, "tex");
                if(VP == -1) printf("\nVP = -1 !!");
                if(tex == -1) printf("\ntex = -1 !!");
            }

            void setVP(glm::mat4 vp){
                glUniformMatrix4fv(VP, 1, GL_FALSE, glm::value_ptr(vp));
            }

            void setTex(){
                glUniform1i(tex, 0);
            }

            void bind(){
                shader.bind();
            }

    };

    class SkyBoxRenderer{
        private:
            static ByteArray boxVB;
            static VertexArray box;
            static bool initialized;
            static SkyBoxShader shader;

        public:
            

            static void initialize(){
                if(!initialized)
                {
                    Array<unsigned int> IB(36);
                    for(unsigned int i = 0; i < 36; i++)
                        IB.add(i);
                    Map<int, Type> attr = Map<int, Type>({3}, FLOAT);
                    box.setBuffers(IB, boxVB, attr);

                    shader.initialize();

                    initialized = true;
                } 
            }

            static void drawSkyBox(SkyBox skyBox, GBuffer * gbuffer, Camera camera){     
                initialize();

                gbuffer->outputBuffer.bind();

                glDisable(GL_DEPTH_TEST);
                glDepthMask(GL_FALSE);

                glDisable(GL_BLEND);

                glDisable(GL_CULL_FACE);

                glEnable(GL_STENCIL_TEST);
                glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
                glStencilMask(0x00);


                shader.bind();
                shader.setVP(camera.getVPWithoutTrans());
                shader.setTex();
                skyBox.bind();
                box.draw();

                gbuffer->outputBuffer.unbind();
            }
    };

    VertexArray     SkyBoxRenderer::box;
    bool            SkyBoxRenderer::initialized = false;
    SkyBoxShader    SkyBoxRenderer::shader;


    String  SkyBoxShader::vertex =   R"====(
                                    layout(location = 0) in vec3 position;

                                    out vec3 texCoord;
                                    uniform mat4 VP;

                                    void main(){
                                        texCoord = position;
                                        gl_Position = VP * vec4(position, 1.0);
                                    }

                                )====";

    String SkyBoxShader::fragment =   R"====(

                                    in vec3 texCoord;
                                    uniform samplerCube tex;

                                    layout(location = 0) out vec4 out_color;

                                    void main(){
                                        out_color = texture(tex, texCoord);
                                    }

                                )====";

    ByteArray SkyBoxRenderer::boxVB = ByteArray::parse({    -1.0f,  1.0f, -1.0f,
                                                            -1.0f, -1.0f, -1.0f,
                                                             1.0f, -1.0f, -1.0f,
                                                             1.0f, -1.0f, -1.0f,
                                                             1.0f,  1.0f, -1.0f,
                                                            -1.0f,  1.0f, -1.0f,

                                                            -1.0f, -1.0f,  1.0f,
                                                            -1.0f, -1.0f, -1.0f,
                                                            -1.0f,  1.0f, -1.0f,
                                                            -1.0f,  1.0f, -1.0f,
                                                            -1.0f,  1.0f,  1.0f,
                                                            -1.0f, -1.0f,  1.0f,

                                                             1.0f, -1.0f, -1.0f,
                                                             1.0f, -1.0f,  1.0f,
                                                             1.0f,  1.0f,  1.0f,
                                                             1.0f,  1.0f,  1.0f,
                                                             1.0f,  1.0f, -1.0f,
                                                             1.0f, -1.0f, -1.0f,

                                                            -1.0f, -1.0f,  1.0f,
                                                            -1.0f,  1.0f,  1.0f,
                                                             1.0f,  1.0f,  1.0f,
                                                             1.0f,  1.0f,  1.0f,
                                                             1.0f, -1.0f,  1.0f,
                                                            -1.0f, -1.0f,  1.0f,

                                                            -1.0f,  1.0f, -1.0f,
                                                             1.0f,  1.0f, -1.0f,
                                                             1.0f,  1.0f,  1.0f,
                                                             1.0f,  1.0f,  1.0f,
                                                            -1.0f,  1.0f,  1.0f,
                                                            -1.0f,  1.0f, -1.0f,

                                                            -1.0f, -1.0f, -1.0f,
                                                            -1.0f, -1.0f,  1.0f,
                                                             1.0f, -1.0f, -1.0f,
                                                             1.0f, -1.0f, -1.0f,
                                                            -1.0f, -1.0f,  1.0f,
                                                             1.0f, -1.0f,  1.0f
                                                        });
}

#endif