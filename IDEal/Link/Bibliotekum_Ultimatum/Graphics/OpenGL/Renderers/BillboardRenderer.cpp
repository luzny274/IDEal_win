#ifndef BU_BILLBOARD_RENDERER
#define BU_BILLBOARD_RENDERER

#include "../Tools/ToolsGL.hpp"

namespace ulm{
    
    #ifndef ULTIMATE_SHADER
    #ifndef BU_2D_RENDERER
        constexpr std::uint_fast16_t Multiplied      { 1 << 1 }; // 0000 0000 0000 0010 | Use color and material multiplier uniforms
    #endif
        constexpr std::uint_fast16_t Instanced       { 1 << 8 }; // 0000 0001 0000 0000 | Instanced drawing
        constexpr std::uint_fast16_t Blended         { 1 << 9 }; // 0000 0010 0000 0000 | Use screen door transparency
        constexpr std::uint_fast16_t Aether          { 1 << 10}; // 0000 0100 0000 0000 | Use additive blending | Billboards submited with this flag are drawn with seperated methon drawAether
    #endif


    class BillboardShader{
        private:
            static String vertex;
            static String fragment;

        public:
            bool initialized = false;
            Shader shader;

            enum UNIFORM_LOCATIONS{
                VP                  = 0,
                right               = 1,
                up                  = 2,
                width               = 3,
                height              = 4,
                textureAtlasLength  = 5,
                textureAtlas        = 6,
                position            = 7,        
                multiplier          = 8,                
                U_Size              = 9
            };

            Array<String> uniform_names = {
                String("VP"),
                String("right"),
                String("up"),
                String("width"),
                String("height"),
                String("textureAtlasLength"),
                String("textureAtlas"),
                String("position"),
                String("multiplier")
            };

            int uniforms[U_Size];

            void initialize(std::uint_fast16_t flags){
                String start = IncludeGLSL::begin;

                if(flags & Multiplied)
                    start += "\n#define MULTIPLY";
                if(flags & Instanced)
                    start += "\n#define INSTANCED";
                if(flags & Blended)
                    start += "\n#define BLENDED";
                if(flags & Aether)
                    start += "\n#define AETHER";
                start += "\n";    

                shader.initialize(start + vertex, start + fragment);
                for(int i = 0; i < U_Size; i++)
                    uniforms[i] = glGetUniformLocation(shader.programID, uniform_names[i].getPtr());

                initialized = true;
            }

            void setVP(glm::mat4 vp, glm::vec3 r, glm::vec3 u){
                glUniformMatrix4fv(uniforms[VP], 1, GL_FALSE, glm::value_ptr(vp));
                glUniform3f(uniforms[right], r.x, r.y, r.z);
                glUniform3f(uniforms[up], u.x, u.y, u.z);                
            }

            void setMultiplier(glm::vec4 mul){
                glUniform4f(uniforms[multiplier], mul.x, mul.y, mul.z, mul.w);
            }

            void setSize(float w, float h){
                glUniform1f(uniforms[width], w);
                glUniform1f(uniforms[height], h);
            }

            void setPosition(glm::vec4 pos){
                glUniform4f(uniforms[position], pos.x, pos.y, pos.z, pos.w);
            }

            void setTextureAtlas(Texture& tex, int len){
                int slot = TextureManager::addToBuffer(tex);
                glUniform1i(uniforms[textureAtlas], slot);
                glUniform1i(uniforms[textureAtlasLength], len);
            }

            void bind() { shader.bind(); }

    };

    class BillboardRenderer{
        private:
            static const int count = 16;
            static BillboardShader shaders[count];
            static Array<Billboard> billboards[count];

            static glm::vec3 right;
            static glm::vec3 up;
            static glm::mat4 VP;

            static bool initialized;
            static GBuffer * gbuffer;

            static std::uint_fast16_t getLocalFlags(std::uint_fast16_t flags){
                std::uint_fast16_t f = 0;
                if(flags & Multiplied)
                    f |= (1 << 0);
                if(flags & Instanced)
                    f |= (1 << 1);
                if(flags & Blended)
                    f |= (1 << 2);
                if(flags & Aether)
                    f |= (1 << 3);
                return f;
            }

            static std::uint_fast16_t getFlags(std::uint_fast16_t localFlags){
                std::uint_fast16_t f = 0;
                if(localFlags & (1 << 0))
                    f |= Multiplied;
                if(localFlags & (1 << 1))
                    f |= Instanced;
                if(localFlags & (1 << 2))
                    f |= Blended;
                if(localFlags & (1 << 3))
                    f |= Aether;
                return f;
            }

            static void draw(std::uint_fast16_t localFlags){
                std::uint_fast16_t flags = getFlags(localFlags);

                if(billboards[localFlags].size == 0)
                    return;

                BillboardShader * shader = &shaders[localFlags];

                if(!shader->initialized)
                    shader->initialize(flags);

                shader->bind();

                for(Billboard& billboard : billboards[localFlags]){
                    if(!(flags & Instanced))
                        shader->setPosition(billboard.position);
                    else
                        billboard.positionBuffer.bind();
                    if(flags & Multiplied)
                        shader->setMultiplier(billboard.multiplier);
                    shader->setSize(billboard.width, billboard.height);
                    shader->setVP(VP, right, up);
                    shader->setTextureAtlas(billboard.textureAtlas, billboard.textureAtlasLength);

                    billboard.va.draw();
                }    
            }

        public:
            static void initialize(){
                if(!initialized)
                    for(std::uint_fast16_t f = 0; f < count; f++)
                        shaders[f].initialize(getFlags(f));

                initialized = true;
            }
            static void emptyArrays(){
                for(std::uint_fast16_t f = 0; f < count; f++)
                    billboards[f].empty(billboards[f].size);
            }

            static void begin(Camera& camera, GBuffer * arg_gbuffer){
                initialize();
                gbuffer = arg_gbuffer;

                VP = camera.getVP();
                up = camera.getUp();
                right = camera.getRight();

                emptyArrays();
            }

            static void submit(Billboard& billboard, std::uint_fast16_t flags){
                if(!billboard.initialized)
                    billboard.initialize();
                
                billboards[getLocalFlags(flags)].add(billboard);
            }

            static void draw(){                
                gbuffer->bind();

                glEnable(GL_DEPTH_TEST);
                glDepthMask(GL_TRUE);
                glDepthFunc(GL_LESS);  

                glDisable(GL_BLEND);

                glDisable(GL_CULL_FACE);

                glEnable(GL_STENCIL_TEST);
                glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
                glStencilMask(0xFF);
                glStencilFunc(GL_ALWAYS, 1, 0xFF);


                for(std::uint_fast16_t i = 0; i < count / 2; i++)
                    draw(i);

                gbuffer->unbind();
            }
            static void drawAether(){         
                gbuffer->outputBuffer.bind();

                glEnable(GL_DEPTH_TEST);
                glDepthMask(GL_FALSE);
                glDepthFunc(GL_LESS); 

                glEnable(GL_BLEND);
                glBlendEquation(GL_FUNC_ADD);
                glBlendFunc(GL_ONE, GL_ONE);

                glDisable(GL_CULL_FACE);  

                glDisable(GL_STENCIL_TEST);
                glStencilMask(0x00);

                for(std::uint_fast16_t i = count / 2; i < count; i++)
                    draw(i);

                gbuffer->outputBuffer.unbind();
            }
    };

    //int                 BillboardRenderer::count = 8;
    BillboardShader     BillboardRenderer::shaders[count];
    Array<Billboard>    BillboardRenderer::billboards[count];
    glm::vec3           BillboardRenderer::right;
    glm::vec3           BillboardRenderer::up;
    glm::mat4           BillboardRenderer::VP(1.f);
    bool                BillboardRenderer::initialized = false;
    GBuffer *           BillboardRenderer::gbuffer = NULL;

    String  BillboardShader::vertex =   R"====(
                                    layout(location = 0) in vec2 coord;

                                    uniform mat4 VP;

                                    uniform vec3 right;
                                    uniform vec3 up;

                                    uniform float width;
                                    uniform float height;

                                    #ifndef INSTANCED
                                        uniform vec4 position;
                                    #else
                                        layout(location = 1) in vec4 position;
                                    #endif

                                    out vec2 texCoord;
                                    out float lifeTime;

                                    void main(){
                                        texCoord = coord;
                                        lifeTime = position.w;
                                        //gl_Position = VP * vec4(position.xyz + vec3(1.0, 0.0, 0.0) * coord.x * 5.0 + vec3(0.0, 1.0, 0.0) * coord.y * 5.0, 1.0);
                                        gl_Position = VP * vec4(position.xyz + right * coord.x * width + up * coord.y * height, 1.0);
                                    }

                                )====";

    String BillboardShader::fragment =   R"====(

                                    uniform int textureAtlasLength;
                                    uniform sampler2D textureAtlas;

                                    uniform vec4 multiplier;

                                    in vec2 texCoord;
                                    in float lifeTime;

                                    layout(location = 0) out vec4 out_color;
                                    #ifndef AETHER
                                        layout(location = 1) out vec4 out_material;
                                        layout(location = 2) out vec4 out_normal;
                                    #endif

                                    void main(){
                                        float phaseCnt = float(textureAtlasLength * textureAtlasLength - 1);
                                        int i = int(phaseCnt * lifeTime);
                                        float coef = (phaseCnt * lifeTime - float(i));

                                        vec2 coord1 = vec2(float(i % textureAtlasLength), float(i / textureAtlasLength)) / float(textureAtlasLength) + (texCoord / float(textureAtlasLength));
                                        i++;
                                        vec2 coord2 = vec2(float(i % textureAtlasLength), float(i / textureAtlasLength)) / float(textureAtlasLength) + (texCoord / float(textureAtlasLength));
                                        
                                        out_color = mix(texture(textureAtlas, coord1), texture(textureAtlas, coord2), coef);
                                        #ifdef MULTIPLY
                                            out_color *= multiplier;
                                        #endif
                                        #ifdef BLENDED
                                            mat4 threshold = mat4
                                            (
                                                1.0 / 17.0,   9.0 / 17.0,   3.0 / 17.0,   11.0 / 17.0,
                                                13.0 / 17.0,  5.0 / 17.0,   15.0 / 17.0,  7.0 / 17.0,
                                                4.0 / 17.0,   12.0 / 17.0,  2.0 / 17.0,   10.0 / 17.0,
                                                16.0 / 17.0,  8.0 / 17.0,   14.0 / 17.0,  6.0 / 17.0
                                            );
                                            int x = int(gl_FragCoord.x);
                                            int y = int(gl_FragCoord.y);
                                            if (out_color.w < threshold[x % 4][y % 4])
                                                discard;
                                        #endif

                                        #ifndef AETHER
                                            out_material = vec4(0.0);
                                            out_normal = vec4(0.5);
                                        #endif
                                    }

                                )====";
}

#endif