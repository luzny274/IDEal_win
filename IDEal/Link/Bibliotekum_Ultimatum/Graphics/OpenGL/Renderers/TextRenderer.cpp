#ifndef BU_MSDF_TEXT_RENDERER
#define BU_MSDF_TEXT_RENDERER

#include "../Tools/ToolsGL.hpp"

namespace ulm{

    enum FontType{ MSDF = 0, SDF = 1};

    class FontParams{
        public:
            float width[256];
            float height[256];
            float bx[256];
            float by[256];
            float advance[256];
    };

    class Font{
        private:
            unsigned int texture = 0;

        public:
            FontType type;
            FontParams fontParams;

            Font(){}

            void bind(){
                TextureManager::activateSlot(0);
                glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
            }

            Font(const char * pathToTxt){
                FILE * f = fopen(pathToTxt, "r");

                char buf[8];
                fscanf(f, "%s", buf);
                if(buf[0] == 'm')
                    type = MSDF;
                else
                    type = SDF;

                fontParams.width[0] = 0.f; fontParams.height[0] = 0.f; fontParams.bx[0] = 0.f; fontParams.by[0] = 0.f; fontParams.advance[0] = 0.f;

                for(int i = 1; i < 256; i++){
                    fscanf(f, "%f;",  &fontParams.width[i]);
                    fscanf(f, "%f;",  &fontParams.height[i]);
                    fscanf(f, "%f;",  &fontParams.bx[i]);
                    fscanf(f, "%f;",  &fontParams.by[i]);
                    fscanf(f, "%f\n", &fontParams.advance[i]);
                }


                GLsizei width = 32;
                GLsizei height = 32;
                GLsizei layerCount = 256;
                GLsizei mipLevelCount = 6;

                glGenTextures(1,&texture);
                glBindTexture(GL_TEXTURE_2D_ARRAY,texture);

                int format;
                int internalFormat;
                if(type == MSDF){
                    format = GL_RGB;
                    internalFormat = GL_RGB8;
                }
                else{
                    format = GL_RED;
                    internalFormat = GL_R8;
                }

                char path[512];
                char name[128];

                int poz;
                for(poz = 0; poz < 512; poz++)
                    if(pathToTxt[poz] == '\0')
                        break;
                
                int poz2;
                for(poz2 = poz - 1; poz2 >= 0; poz2--)
                    if(pathToTxt[poz2] == '.')
                        break;

                int poz3;
                for(poz3 = poz2 - 1; poz3 >= 0; poz3--)
                    if(pathToTxt[poz3] == '/' || pathToTxt[poz3] == '\\')
                        break;

                int i;
                for(i = 0; i < poz3; i++)
                    path[i] = pathToTxt[i];
                
                path[i] = '\0';

                for(i = poz3; i < poz2; i++)
                    name[i - poz3] = pathToTxt[i];
                name[i - poz3] = '\0';

                int w, h, c;

                for(i = 0; i < mipLevelCount; i++){
                    sprintf(path + poz3, "%s_%d.png", name, i);

                    unsigned char * data = stbi_load(path, &w, &h, &c, 0);
                    if(data == NULL)
                        Properties::handleError("ULM::ERROR::Failed to load font");

                    glTexImage3D(GL_TEXTURE_2D_ARRAY, i, internalFormat, width, height, layerCount, 0, format, GL_UNSIGNED_BYTE, data);
                    width /= 2;
                    height /= 2;

                    free(data);
                }

                glTexParameteri(GL_TEXTURE_2D_ARRAY,    GL_TEXTURE_MIN_FILTER,  GL_LINEAR_MIPMAP_NEAREST);
                glTexParameteri(GL_TEXTURE_2D_ARRAY,    GL_TEXTURE_MAG_FILTER,  GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D_ARRAY,    GL_TEXTURE_WRAP_S,      GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D_ARRAY,    GL_TEXTURE_WRAP_T,      GL_CLAMP_TO_EDGE);

                glTexParameteri(GL_TEXTURE_2D_ARRAY,    GL_TEXTURE_BASE_LEVEL,  0);
                glTexParameteri(GL_TEXTURE_2D_ARRAY,    GL_TEXTURE_MAX_LEVEL,   (mipLevelCount - 1));
                
            }
    };

    class TextBlock{
        private:
            unsigned int va  = 0;
            unsigned int vb  = 0;
            unsigned int ivb = 0;
            unsigned int ib  = 0;

            String text;

            glm::vec2 position;
            float lineLength;
            float lineGapSize;

            float gapScale;
            
            float fontSize;
            float aspectRatio;

            void init(){
                if(fontParams == NULL){
                    Properties::handleError("ULM::ERROR::TextBlock::SetText::NOT::INITIALIZED");
                    return;
                }

                if(va == 0){
                    glGenVertexArrays(1, &va);
                    glGenBuffers(1, &vb);
                    glGenBuffers(1, &ivb);
                    glGenBuffers(1, &ib);
                }
                glBindVertexArray(va);

                /* Instanced character bounding box */
                ByteArray vbData(4);

                vbData.add((unsigned char)0  ); vbData.add((unsigned char)255);

                vbData.add((unsigned char)0  ); 
                vbData.add((unsigned char)0  );

                vbData.add((unsigned char)0  ); vbData.add((unsigned char)0  );   

                vbData.add((unsigned char)0  ); 
                vbData.add((unsigned char)(fontSize * 255.f));    

                vbData.add((unsigned char)255); vbData.add((unsigned char)255);

                vbData.add((unsigned char)((fontSize * 255.f) / aspectRatio)); 
                vbData.add((unsigned char)0);  

                vbData.add((unsigned char)255); vbData.add((unsigned char)0  );

                vbData.add((unsigned char)((fontSize * 255.f) / aspectRatio)); 
                vbData.add((unsigned char)(fontSize * 255.f));  

                glBindBuffer(GL_ARRAY_BUFFER, vb);
                glBufferData(GL_ARRAY_BUFFER, vbData.size * sizeof(unsigned char), vbData.getPtr(), GL_STATIC_DRAW);

                glVertexAttribPointer(0, 4, GL_UNSIGNED_BYTE, GL_TRUE, 4 * sizeof(unsigned char), 0);
                glEnableVertexAttribArray(0);
                
                /* Vertex Buffer containing position and character information */

                int stride = 2 * sizeof(float) + 2 * sizeof(unsigned char);
                ByteArray ivbData(getSize() * stride * 4);

                float radek = position.y;
                float curX = position.x;


                for(int i = 0; i < getSize(); i++){
                    glm::vec2 pos(curX, radek);
                    
                    pos.x += (fontParams->bx[(unsigned char)text[i]]) * fontSize / aspectRatio;
                    //pos.y -= (fontParams->height[(unsigned char)text[i]] / 2.f - fontParams->by[(unsigned char)text[i]]) * fontSize;

                    float scale = Math::max(fontParams->width[(unsigned char)text[i]], fontParams->height[(unsigned char)text[i]]);
                    pos.y -= (fontParams->height[(unsigned char)text[i]] / 2.f - fontParams->by[(unsigned char)text[i]]) * fontSize;
                    pos -= fontSize * scale / 2.f;

                    ivbData.add(pos.x);   ivbData.add(pos.y);

                    ivbData.add((unsigned char)text[i]); 
                    ivbData.add((unsigned char)(scale * 255.f));

                    curX += (fontParams->advance[(unsigned char)text[i]] + fontParams->bx[(unsigned char)text[i]]) * fontSize / aspectRatio * gapScale;

                    if(curX >= lineLength){
                        curX = position.x;
                        radek -= fontSize + lineGapSize;
                    }
                }


                Array<unsigned int> indices(getSize() * 6);

                for(int i = 0; i < getSize(); i++){
                    indices.add(0 + i * 4);
                    indices.add(3 + i * 4);
                    indices.add(1 + i * 4);
                    indices.add(0 + i * 4);
                    indices.add(3 + i * 4);
                    indices.add(2 + i * 4);
                }


                glBindBuffer(GL_ARRAY_BUFFER, ivb);
                glBufferData(GL_ARRAY_BUFFER, ivbData.size * sizeof(unsigned char), ivbData.getPtr(), GL_DYNAMIC_DRAW);

                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,          stride, (void*)(0));
                glEnableVertexAttribArray(1);

                glVertexAttribPointer(2, 1,  GL_UNSIGNED_BYTE, GL_FALSE, stride, (void*)(2 * sizeof(float)));
                glEnableVertexAttribArray(2);

                glVertexAttribPointer(3, 1,  GL_UNSIGNED_BYTE, GL_TRUE,  stride, (void*)(2 * sizeof(float) + sizeof(unsigned char)));
                glEnableVertexAttribArray(3);
                
                glVertexAttribDivisor(1, 1);
                glVertexAttribDivisor(2, 1);
                glVertexAttribDivisor(3, 1);

                /* Index buffer */
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices.getPtr(), GL_STATIC_DRAW);

                glBindVertexArray(Window::vertexArray);                
            }

        public:
            FontParams * fontParams = NULL;

            TextBlock(){}

            TextBlock(FontParams * arg_fontParams, glm::vec2 arg_position, float arg_fontSize, float arg_lineLength, float arg_lineGapSize, float arg_gapScale, float arg_aspectRatio){ initialize(arg_fontParams, arg_position, arg_fontSize, arg_lineLength, arg_lineGapSize, arg_gapScale, arg_aspectRatio); }
            
            TextBlock(FontParams * arg_fontParams, glm::vec2 arg_position, float arg_fontSize, float arg_lineLength, float arg_aspectRatio){ initialize(arg_fontParams, arg_position, arg_fontSize, arg_lineLength, arg_aspectRatio); }
            TextBlock(FontParams * arg_fontParams, glm::vec2 arg_position, float arg_fontSize, float arg_aspectRatio){ initialize(arg_fontParams, arg_position, arg_fontSize, arg_aspectRatio); }

            void initialize(FontParams * arg_fontParams, glm::vec2 arg_position, float arg_fontSize, float arg_aspectRatio){ initialize(arg_fontParams, arg_position, arg_fontSize, 2.f, arg_aspectRatio); }
            void initialize(FontParams * arg_fontParams, glm::vec2 arg_position, float arg_fontSize, float arg_lineLength, float arg_aspectRatio){ initialize(arg_fontParams, arg_position, arg_fontSize, arg_lineLength, 0.f, 0.f, arg_aspectRatio); }

            void initialize(FontParams * arg_fontParams, glm::vec2 arg_position, float arg_fontSize, float arg_lineLength, float arg_lineGapSize, float arg_gapScale, float arg_aspectRatio){
                fontParams  = arg_fontParams;
                position    = arg_position;
                lineLength  = arg_lineLength;
                fontSize    = arg_fontSize;
                lineGapSize = arg_lineGapSize;
                gapScale     = arg_gapScale;
                aspectRatio = arg_aspectRatio;
            }

            int getSize(){
                return(text.array.size - 1);
            }

            void destroy(){
                if(va != 0){
                    glDeleteVertexArrays(1, &va);
                    glDeleteBuffers(1, &vb);
                    glDeleteBuffers(1, &ivb);
                    glDeleteBuffers(1, &ib);
                }
            }

            void setText(String arg_text){ setText(arg_text.getPtr()); }

            void setText(const char * arg_text){
                text = arg_text;
                init();
            }

            unsigned int getVA(){ return va; }
    };

    class TextRenderer{
        private:
            static String vertex;
            static String fragment;

            static Shader msdfShader;
            static Shader sdfShader;
            static bool initialized;

            static Map<Font*, Map<unsigned int, unsigned int>> msdfBlocks; // font, (va, size)
            static Map<Font*, Map<unsigned int, unsigned int>> sdfBlocks; //  font, (va, size)

            static void submitMSDF(Font * font, TextBlock& textBlock){
                int i = 0;
                for(i = 0; i < msdfBlocks.size; i++){
                    if(msdfBlocks[i].x == font){
                        msdfBlocks[i].y.add(Couple<unsigned int, unsigned int>(textBlock.getVA(), textBlock.getSize()));
                        return;
                    }
                }

                Couple<Font*, Map<unsigned int, unsigned int>> el;
                el.x = font;
                el.y = Map<unsigned int, unsigned int>();
                el.y.add(Couple<unsigned int, unsigned int>(textBlock.getVA(), textBlock.getSize()));

                msdfBlocks.add(el);

            }
            
            static void submitSDF(Font * font, TextBlock& textBlock){
                int i = 0;
                for(i = 0; i < sdfBlocks.size; i++){
                    if(sdfBlocks[i].x == font){
                        sdfBlocks[i].y.add(textBlock.getVA());
                        return;
                    }
                }

                Couple<Font*, Map<unsigned int, unsigned int>> el;
                el.x = font;
                el.y = Map<unsigned int, unsigned int>();
                el.y.add(Couple<unsigned int, unsigned int>(textBlock.getVA(), textBlock.getSize()));

                sdfBlocks.add(el);
            }

        public:
            static float      width;
            static float      borderWidth;
            static glm::vec2  shadowDir;
            static glm::vec3  color;
            static glm::vec3  borderColor;
            static float  borderLineWidth;

            static void initialize(){
                initialized = true;
                
                msdfShader.initialize(IncludeGLSL::begin + vertex, IncludeGLSL::begin + "\n#define MSDF\n" + fragment);
                sdfShader.initialize(IncludeGLSL::begin + vertex, IncludeGLSL::begin + fragment);
            }

            static void begin(){
                if(!initialized)
                    initialize();

                msdfBlocks.empty(msdfBlocks.size);
                sdfBlocks.empty(sdfBlocks.size);

                glDisable(GL_STENCIL_TEST);
                glStencilMask(0x00);

                glDisable(GL_CULL_FACE);  
                glDisable(GL_DEPTH_TEST);
                glDepthMask(GL_FALSE);

                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            }

            static void submit(Font * font, TextBlock& textBlock){
                if(textBlock.getVA() != 0){
                    if(font->type == MSDF)
                        submitMSDF(font, textBlock);
                    else
                        submitSDF(font, textBlock);
                }
            }

            static void draw(){
                if(msdfBlocks.size > 0){
                    msdfShader.bind();
                    msdfShader.setInt("textures", 0);
                    msdfShader.setVec3("color", color);
                    msdfShader.setFloat("width", width);
                    msdfShader.setFloat("borderWidth",  borderWidth);
                    msdfShader.setVec2 ("shadowDir",    shadowDir);
                    msdfShader.setVec3 ("borderColor",  borderColor);
                    msdfShader.setFloat ("borderLineWidth",  borderLineWidth);

                    for(int i = 0; i < msdfBlocks.size; i++){
                        msdfBlocks[i].x->bind();
                        for(int ii = 0; ii < msdfBlocks[i].y.size; ii++){
                            glBindVertexArray(msdfBlocks[i].y[ii].x);
                            glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, msdfBlocks[i].y[ii].y);
                        }
                    }
                }

                if(sdfBlocks.size > 0){
                    sdfShader.bind();
                    sdfShader.setInt("textures", 0);
                    sdfShader.setVec3("color", color);
                    sdfShader.setFloat("width", width);
                    sdfShader.setFloat("borderWidth",  borderWidth);
                    sdfShader.setVec2 ("shadowDir",    shadowDir);
                    sdfShader.setVec3 ("borderColor",  borderColor);
                    sdfShader.setFloat ("borderLineWidth",  borderLineWidth);

                    for(int i = 0; i < sdfBlocks.size; i++){
                        sdfBlocks[i].x->bind();
                        for(int ii = 0; ii < sdfBlocks[i].y.size; ii++){
                            glBindVertexArray(sdfBlocks[i].y[ii].x);
                            glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, sdfBlocks[i].y[ii].y);
                        }
                    }
                }

                glBindVertexArray(Window::vertexArray);
            }


    };

    Shader                                      TextRenderer::msdfShader;
    Shader                                      TextRenderer::sdfShader;
    bool                                        TextRenderer::initialized = false;
    Map<Font*, Map<unsigned int, unsigned int>> TextRenderer::msdfBlocks;
    Map<Font*, Map<unsigned int, unsigned int>> TextRenderer::sdfBlocks;
    
    float                                       TextRenderer::width = 0.5f;
    float                                       TextRenderer::borderWidth = 0.5f;
    glm::vec2                                   TextRenderer::shadowDir = glm::vec2(0.f);

    glm::vec3                                   TextRenderer::color = glm::vec3(1.f);
    glm::vec3                                   TextRenderer::borderColor = glm::vec3(0.f);
    float                                       TextRenderer::borderLineWidth = 0.f;


    String TextRenderer::vertex = R"====(
                            #ifdef GL_ES
                                precision mediump float;
                            #endif

                            layout(location = 0) in vec4 aTexCoordAPos; //xy - texCoord, zw - position of vertex

                            layout(location = 1) in vec2 aPos; //position of character
                            layout(location = 2) in float aCharacter;
                            layout(location = 3) in float aScale;

                            out vec3 texCoord;

                            void main()
                            {
                                texCoord = vec3(aTexCoordAPos.xy, aCharacter);

                                gl_Position = vec4(aPos.xy + (aTexCoordAPos.zw * aScale), 1.0, 1.0);
                            }
                            )====";

    String TextRenderer::fragment = R"====(
                            #ifdef GL_ES
                                precision mediump float;
                            #endif

                            in vec3 texCoord;

                            uniform sampler2DArray textures;
                            uniform float width;
                            uniform vec3 color;
                            uniform float borderWidth;
                            uniform float borderLineWidth;
                            uniform vec2 shadowDir;
                            uniform vec3 borderColor;

                            layout(location = 0) out vec4 out_color;

                            #ifdef MSDF
                                float getSigDist(vec3 coords){
                                    vec3 sample = texture(textures, coords).rgb;
                                    return max(min(sample.r, sample.g), min(max(sample.r, sample.g), sample.b));
                                }
                            #else
                                float getSigDist(vec3 coords){
                                    return texture(textures, coords).r;
                                }
                            #endif

                            void main()
                            {
                                float sigDist = 1.0 - getSigDist(texCoord);
                                float w = fwidth(sigDist);

                                float opacity  = 1.0 - smoothstep(width - w, width + w, sigDist);

                                float sigDist2 = 1.0 - getSigDist(vec3(texCoord.xy - shadowDir, texCoord.z));
                                float opacity2 = 1.0 - smoothstep(borderWidth - w, borderWidth + w + borderLineWidth, sigDist2);

                                float overall = opacity + (1.0 - opacity) * opacity2 + 0.01;

                                vec3 c = mix(borderColor, color, opacity / overall);

                                out_color = vec4(c, overall);
                            }
                            )====";

}

#endif