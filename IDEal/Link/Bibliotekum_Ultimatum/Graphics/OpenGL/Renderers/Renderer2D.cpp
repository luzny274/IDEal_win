#ifndef BU_2D_RENDERER
#define BU_2D_RENDERER

#include "../Tools/ToolsGL.hpp"

namespace ulm{

    class ShaderToy : public Shader{
        private:
            static const char * ST_vertex;
            VertexArray ST_va;
            unsigned int ST_posLoc;
            unsigned int ST_sizeLoc;
            unsigned int ST_MVP;

        public:
            String code;
            
            void compile(){
                ByteArray vertices =  ulm::ByteArray::parse({   0.f, 0.f,
                                                                0.f, 1.f,
                                                                1.f, 0.f,
                                                                1.f, 1.f});

                Map<int, Type> attr = Map<int,Type>({2}, FLOAT);

                Array<unsigned int> indices = {0, 3, 1, 0, 3, 2};
                ST_va.setBuffers(indices, vertices, attr);

                code = String(R"====(#version 100
                                #ifdef GL_ES
                                    precision mediump float;
                                #endif
                                )====") + code;
                                
                initialize(ST_vertex, code.getPtr());
                ST_posLoc = glGetUniformLocation(programID,  "BU_pos");
                ST_sizeLoc = glGetUniformLocation(programID, "BU_size");
                ST_MVP = glGetUniformLocation(programID, "BU_MVP");
            }

            void ST_setMVP(glm::mat4 mat){
                glUniformMatrix4fv(ST_MVP, 1, GL_FALSE, glm::value_ptr(mat));
            }

            void ST_setPosition(glm::vec2 position){ glUniform2f(ST_posLoc, position.x, position.y); }
            void ST_setSize(glm::vec2 size){ glUniform2f(ST_sizeLoc, size.x, size.y); }

            void ST_draw(){
                ST_va.draw();
            }

            void ST_bind(){
                bind();
            }
    };

    #ifndef ULTIMATE_SHADER
        constexpr std::uint_fast16_t Colored            { 1 << 0 }; // 0000 0001
    #ifndef BU_BILLBOARD_RENDERER
        constexpr std::uint_fast16_t Multiplied         { 1 << 1 }; // 0000 0010 | Use color and material multiplier uniforms
    #endif
    #endif      
    constexpr std::uint_fast16_t Textured               { 1 << 2 }; // 0000 0100 | Use texture
    constexpr std::uint_fast16_t DepthTextured          { 1 << 3 }; // 0000 1000 | Linearize depth texture

    class Renderer2DShader{
        private:
            static const char * vertex;
            static const char * fragment;

        public:
            unsigned int MVP;
            unsigned int sampler;

            unsigned int colored;
            unsigned int textured;
            unsigned int depth;
            unsigned int multiply;

            unsigned int ZFar;
            unsigned int ZNear;
            unsigned int multiplier;
            
            Shader shader;

            void initialize(){
                shader.initialize(IncludeGLSL::begin + vertex, IncludeGLSL::begin + fragment);
                MVP =           glGetUniformLocation(shader.programID, "MVP");
                sampler =       glGetUniformLocation(shader.programID, "ourTexture");
                colored =       glGetUniformLocation(shader.programID, "colored");
                textured =      glGetUniformLocation(shader.programID, "textured");
                depth =         glGetUniformLocation(shader.programID, "depth");
                multiply =      glGetUniformLocation(shader.programID, "multiply");

                ZFar =          glGetUniformLocation(shader.programID, "ZFar");
                ZNear =         glGetUniformLocation(shader.programID, "ZNear");
                multiplier =    glGetUniformLocation(shader.programID, "multiplier");
            }

            void setMVP(glm::mat4 mat){
                glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(mat));
            }

            void setFlags(std::uint_fast16_t flags){
                if(flags & Colored)         glUniform1i(colored,  1);
                else                        glUniform1i(colored,  0);

                if(flags & Textured)        glUniform1i(textured, 1);
                else                        glUniform1i(textured, 0);

                if(flags & DepthTextured)   glUniform1i(depth,    1);
                else                        glUniform1i(depth,    0);

                if(flags & Multiplied)      glUniform1i(multiply, 1);
                else                        glUniform1i(multiply, 0);
            }

            void setMultiplier(glm::vec4& mult){
                glUniform4f(multiplier, mult.x, mult.y, mult.z, mult.w);
            }

            void setProjPar(float z_near, float z_far){
                glUniform1f(ZNear, z_near);
                glUniform1f(ZFar, z_far);
            }

            void setTextureSlot(int slot){
                glUniform1i(sampler, slot);
            }

            void bind(){
                shader.bind();
            }
    };

    class Renderer2D{
        private:
            static glm::mat4 VP;
            static ShaderToy * ST;

            static bool initialized;
        
        public:
            static Renderer2DShader shader;
            static float ZNear;
            static float ZFar;

            static void initialize(){
                if(!initialized){
                    initialized = true;
                    shader.initialize();
                }
            }

            static void setUp(){
                initialize();
                glDisable(GL_STENCIL_TEST);
                glStencilMask(0x00);

                glDisable(GL_CULL_FACE);  
                glDisable(GL_DEPTH_TEST);
                glDepthMask(GL_FALSE);

                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            }

            static void begin(Camera& arg_camera){
                setUp();
                VP = arg_camera.getVP();
                shader.bind();
            }

            static void begin(glm::mat4 arg_VP){
                setUp();

                VP = arg_VP;
                shader.bind();
            }

            static void draw(Sprite& sprite, std::uint_fast16_t flags){

                if(!sprite.initialized)
                    Properties::handleError("ULM::ERROR::Renderer2D::draw::sprite is not initialized");

                if(flags >= 2 * DepthTextured)
                    Properties::handleError("ULM::ERROR::Renderer2D::draw::invalid flags");
                

                
                glm::mat4 MVP = VP * sprite.modelMat;
                shader.setMVP(MVP);
                shader.setFlags(flags);

                //if(flags & Multiplied)
                    shader.setMultiplier(sprite.colorMultiplier);

                //if(flags & DepthTextured)
                    shader.setProjPar(ZNear, ZFar);

                if(flags & Textured || flags & DepthTextured){
                    if(sprite.texture.id == 0)
                        Properties::handleError(String("ULM::WARNING::RENDERER2D::Sprite has no texture"));
                    int slot = TextureManager::use(sprite.texture);
                    shader.setTextureSlot(slot);
                }
            
                sprite.vertexArray.draw();
            }

            static void end(){}

            static void beginST(ShaderToy& shaderToy){
                setUp();
                ST = &shaderToy;
                ST->ST_bind();
            }

            static void drawST(float x, float y, float w, float h, glm::mat4 model, glm::mat4 VP){
                ST->ST_setMVP(VP * model);
                ST->ST_setPosition(glm::vec2(x, y));
                ST->ST_setSize(glm::vec2(w, h));
                ST->ST_draw();
            }

            static void drawST(float x, float y, float w, float h, glm::mat4 model, Camera& camera) { drawST(x, y, w, h, model,             camera.getVP()); }
            static void drawST(float x, float y, float w, float h, glm::mat4 model)                 { drawST(x, y, w, h, model,             glm::mat4(1.f)); }
            static void drawST(float x, float y, float w, float h)                                  { drawST(x, y, w, h, glm::mat4(1.f),    glm::mat4(1.f)); }

            static void endST(){}

    };

    ShaderToy *         Renderer2D::ST = NULL;
    glm::mat4           Renderer2D::VP = glm::mat4(1.0f);
    bool                Renderer2D::initialized = false;
    Renderer2DShader    Renderer2D::shader;
    float               Renderer2D::ZNear = 0.1f;
    float               Renderer2D::ZFar = 1000.0f;

    class STCodeExamples{
        public:

            /* uniform names: pozice, polomer, tloustka, barvicka */
            static String circle;

            /* uniform names: pozice, polomer, barvicka */
            static String filledCircle;

            /* missing definition of macro PALETTE_SIZE, uniform names: center, scale, iter, palette[PALETTE_SIZE]*/
            static String Mandelbrot;

            /* missing definition of macro PALETTE_SIZE, uniform names: seed, center, iter, scale, palette[PALETTE_SIZE]*/
            static String JuliaSet;
    };

    String STCodeExamples::circle = R"====(
                        varying vec2 position;

                        uniform vec2 pozice;
                        uniform float polomer;
                        uniform float tloustka;
                        uniform vec4 barvicka;

                        void main()
                        {
                            if(length(position - pozice) < (polomer + tloustka) && length(position - pozice) > (polomer - tloustka)){
                                gl_FragColor = barvicka;
                            }else{
                                gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
                            }
                        })====";

    String STCodeExamples::filledCircle = R"====(
                varying vec2 position;

                uniform vec2 pozice;
                uniform float polomer;
                uniform vec4 barvicka;                      

                void main()
                {
                    if(length(position - pozice) < polomer){
                        gl_FragColor = barvicka;
                    }else{
                        gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
                    }
                })====";

    String STCodeExamples::Mandelbrot = R"====(
                varying vec2 position;

                uniform vec2 center;
                uniform float scale;
                uniform int iter;
                uniform vec4 palette[PALETTE_SIZE];

                void main() {
                    vec2 z, c;

                    c.x = 1.3333 * (position.x - 0.5) * scale - center.x;
                    c.y = (position.y) * scale - center.y;

                    int i;
                    z = c;
                    for(i=0; i<iter; i++) {
                        float x = (z.x * z.x - z.y * z.y) + c.x;
                        float y = (z.y * z.x + z.x * z.y) + c.y;

                        if((x * x + y * y) > 4.0) break;
                        z.x = x;
                        z.y = y;
                    }
                    float cislo = sqrt((i == iter ? 0.0 : float(i)) / float(iter));
                    int i1 = int(float(PALETTE_SIZE - 1) * cislo);
                    int i2 = int(float(PALETTE_SIZE - 1) * cislo + 1.0);

                    float pomer = (float(i2) - cislo * float(PALETTE_SIZE - 1));
                    gl_FragColor = mix(palette[i1], palette[i2], 1.0 - pomer);                            
                })====";

    String STCodeExamples::JuliaSet = R"====(
                varying vec2 position;

                uniform vec2 seed;
                uniform int iter;
                uniform vec2 center;
                uniform float scale;

                uniform vec4 palette[PALETTE_SIZE];

                void main() {
                    vec2 z;
                    z.x = 3.0 * (position.x) / scale - center.x;
                    z.y = 2.0 * (position.y) / scale - center.y;

                    int i;
                    for(i=0; i<iter; i++) {
                        float x = (z.x * z.x - z.y * z.y) + seed.x;
                        float y = (z.y * z.x + z.x * z.y) + seed.y;

                        if((x * x + y * y) > 4.0) break;
                        z.x = x;
                        z.y = y;
                    }
                    float cislo = sqrt((i == iter ? 0.0 : float(i)) / float(iter));
                    if(cislo > 1.0) cislo = 1.0;
                    
                    int i1 = int(float(PALETTE_SIZE - 1) * cislo);
                    int i2 = int(float(PALETTE_SIZE - 1) * cislo + 1.0);

                    float pomer = (float(i2) - cislo * float(PALETTE_SIZE - 1));
                    gl_FragColor = mix(palette[i1], palette[i2], 1.0 - pomer);
                })====";


    /* 2D renderer shader */

    const char * Renderer2DShader::vertex =     R"====(

                                                    layout(location = 0) in vec2 aPos;
                                                    layout(location = 1) in vec4 aColor;
                                                    layout(location = 2) in vec2 aTexCoord;

                                                    out vec4 Color;
                                                    out vec2 TexCoord;

                                                    uniform mat4 MVP;

                                                    void main()
                                                    {
                                                        Color = aColor;
                                                        TexCoord = aTexCoord;
                                                        gl_Position = MVP * vec4(aPos.x, aPos.y, 1.0, 1.0);
                                                    }
                                                )====";

    const char * Renderer2DShader::fragment =   R"====(
                                                    in vec4 Color;
                                                    in vec2 TexCoord;

                                                    uniform sampler2D ourTexture;
                                                    uniform bool colored;
                                                    uniform bool textured;
                                                    uniform bool depth;
                                                    uniform bool multiply;

                                                    uniform float ZNear;
                                                    uniform float ZFar;
                                                    
                                                    uniform vec4 multiplier;

                                                    layout(location = 0) out vec4 out_color;

                                                    float linearDepth(){
                                                        float pom = 2.0 * texture(ourTexture, TexCoord).x - 1.0;
                                                        return( 2.0 * ZNear * ZFar / (ZFar + ZNear - pom * (ZFar - ZNear)) );
                                                    }

                                                    void main()
                                                    {
                                                        vec4 color;

                                                        if(!colored && !textured && !depth)
                                                            color = vec4(1.0);
                                                        else
                                                            color = vec4(0.0);

                                                        if(colored)
                                                            color += Color;
                                                        if(textured)
                                                            color += texture(ourTexture, TexCoord);
                                                        if(depth)
                                                            color += vec4(vec3(linearDepth()), 1.0);
                                                        
                                                        if(multiply)
                                                            color *= multiplier;

                                                        out_color = color;
                                                        //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
                                                    }
                                                )====";






    /* 2DShaderToy vertex */

    const char * ShaderToy::ST_vertex = R"====(
                                        #version 100
                                        #ifdef GL_ES
                                            precision mediump float;
                                        #endif

                                        attribute vec2 vertex;

                                        varying vec2 position;
                                        
                                        uniform vec2 BU_pos;
                                        uniform vec2 BU_size;

                                        uniform mat4 BU_MVP;

                                        void main()
                                        {
                                            position = vec2(vertex * 2.0 - 1.0);
                                            
                                            gl_Position = BU_MVP * vec4(vertex * BU_size + BU_pos, 1.0, 1.0);
                                        }
                                        )====";

}

#endif
