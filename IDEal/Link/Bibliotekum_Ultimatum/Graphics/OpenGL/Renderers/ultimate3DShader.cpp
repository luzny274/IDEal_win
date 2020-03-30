#ifndef ULTIMATE_SHADER
#define ULTIMATE_SHADER

#include "../Tools/ToolsGL.hpp"
#include "Renderer3D.cpp"

namespace ulm{
    #ifndef BU_2D_RENDERER
        constexpr std::uint_fast16_t Colored         { 1 << 0 }; // 0000 0000 0000 0001 | Use per-vertex color and material values
    #ifndef BU_BILLBOARD_RENDERER
        constexpr std::uint_fast16_t Multiplied      { 1 << 1 }; // 0000 0000 0000 0010 | Use color and material multiplier uniforms
    #endif
    #endif
        constexpr std::uint_fast16_t DiffuseMapped   { 1 << 2 }; // 0000 0000 0000 0100 | Use diffuse map
        constexpr std::uint_fast16_t AlbedoMapped    { 1 << 3 }; // 0000 0000 0000 1000 | Use material map
        constexpr std::uint_fast16_t NormalMapped    { 1 << 4 }; // 0000 0000 0001 0000 | Use normal map
        constexpr std::uint_fast16_t ParallaxMapped  { 1 << 5 }; // 0000 0000 0010 0000 | Use parallax map
        constexpr std::uint_fast16_t Animated        { 1 << 6 }; // 0000 0000 0100 0000 | Use bone animation
        constexpr std::uint_fast16_t ForcePrecision  { 1 << 7 }; // 0000 0000 1000 0000 | Force usage of transposed inverted model matrix for transformation of a normal vector
    #ifndef BU_BILLBOARD_RENDERER
        constexpr std::uint_fast16_t Instanced       { 1 << 8 }; // 0000 0001 0000 0000 | Instanced drawing
        constexpr std::uint_fast16_t Blended         { 1 << 9 }; // 0000 0010 0000 0000 | Use screen door transparency
        constexpr std::uint_fast16_t Aether          { 1 << 10}; // 0000 0100 0000 0000 | Use additive blending | Meshes submited with this flag are drawn with seperated methon drawAether
    #endif

    class UltimateShader{
        private:
            static const char * vertex;
            static const char * fragment;

            static int maxUniformMatArray;

            static int getMaxUniformMatArray(){
                if(maxUniformMatArray > 0)
                    return maxUniformMatArray;
                else{
                    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &maxUniformMatArray);
                    maxUniformMatArray /= 16;
                    return maxUniformMatArray;
                }
            }

        public:
            Shader shader;
            bool initialized = false;
            int skeleton_max = 2;
            int skeleton_buffer_max = 2;

            std::uint_fast16_t drawFlags = 0;

            enum UNIFORM_LOCATIONS{
                MVP                 = 0,
                colorTex            = 1,
                TIModel             = 2,
                matTex              = 3,
                normTex             = 4,
                heightTex           = 5,
                viewPos             = 6,
                model               = 7,
                minLayers           = 8,
                maxLayers           = 9,
                heightScale         = 10,
                repeatTexture       = 11,
                colorMultiplier     = 12,
                materialMultiplier  = 13,
                VP                  = 14,
                bones               = 15,
                skeleton_size       = 16,
                U_Size              = 17
            };

            Array<String> uniform_names = {
                String("MVP"),
                String("colorTex"),
                String("TIModel"),
                String("matTex"),
                String("normTex"),
                String("heightTex"),
                String("viewPos"),
                String("u_model"),
                String("minLayers"),
                String("maxLayers"),
                String("heightScale"),
                String("repeatTexture"),
                String("colorMultiplier"),
                String("materialMultiplier"),
                String("VP"),
                String("bones[0]"),
                String("skeleton_size")
            };

            int uniforms[U_Size];

            void initialize(std::uint_fast16_t flags){
                drawFlags = flags;
                initialized = true;
                String start = IncludeGLSL::begin;

                if(flags & Colored)
                    start += "\n#define COLORED";
                if(flags & Multiplied)
                    start += "\n#define MULTIPLY";
                if(flags & DiffuseMapped)
                    start += "\n#define DIFFUSE_MAPPED";
                if(flags & AlbedoMapped)
                    start += "\n#define MATERIAL_MAPPED";
                if(flags & NormalMapped)
                    start += "\n#define NORMAL_MAPPED";
                if(flags & ParallaxMapped)
                    start += "\n#define PARALLAX_MAPPED";
                if(flags & Animated)
                    start += "\n#define ANIMATED";
                if(flags & ForcePrecision)
                    start += "\n#define FORCE_PRECISION";
                if(flags & Instanced)
                    start += "\n#define INSTANCED";
                if(flags & Blended)
                    start += "\n#define BLENDED";
                if(flags & Aether)
                    start += "\n#define AETHER";

                if(!(flags & Instanced) && (flags & Animated))
                    start += String("\n#define SKELETON_MAX ") + skeleton_max;
                else if((flags & Instanced) && (flags & Animated))
                    start += String("\n#define SKELETON_BUFFER_MAX ") + skeleton_buffer_max;

                start += "\n";

                

                shader.initialize(start + vertex, start + fragment);

                for(int i = 0; i < U_Size; i++){
                    uniforms[i] = glGetUniformLocation(shader.programID, uniform_names[i].getPtr());
                    //if(uniforms[i] == -1)
                    //    printf("\nULM::SHADER::ERROR::UNIFORM_NOT_FOUND::%s", uniform_names[i].getPtr());
                }
                if((flags & Animated) && (flags & Instanced))
                    initSkeletonBuffer();

            }

            void initSkeletonBuffer(){
                glUniformBlockBinding(shader.programID, glGetUniformBlockIndex(shader.programID, "SkeletonBlock"), 0);
            }

            void setBones(Array<glm::mat4> boneArray){
                if(boneArray.size >= skeleton_max){
                    while(boneArray.size >= skeleton_max)
                        skeleton_max *= 2;

                    initialize(drawFlags);
                }

                glUniformMatrix4fv(uniforms[bones], boneArray.size, GL_FALSE, glm::value_ptr(boneArray.getPtr()[0]));
                
            }

            void setSkeletonBuffer(SkeletonBuffer sb){
                if((int)sb.size >= skeleton_buffer_max && (skeleton_buffer_max * 2 <= getMaxUniformMatArray())){
                    while((int)sb.size >= skeleton_buffer_max && (skeleton_buffer_max * 2 <= getMaxUniformMatArray()))
                        skeleton_buffer_max *= 2;

                    initialize(drawFlags);
                }
                sb.bind();
                glUniform1i(uniforms[skeleton_size], sb.skeletonSize);//*/
            }

            void setMVP(glm::mat4 m, glm::mat4 vp, bool sendModel){
                glm::mat4 mvp = vp * m;
                glUniformMatrix4fv(uniforms[MVP], 1, GL_FALSE, glm::value_ptr(mvp));

                glm::mat3 ti = glm::mat3(glm::transpose(glm::inverse(m)));
                glUniformMatrix3fv(uniforms[TIModel], 1, GL_FALSE, glm::value_ptr(ti));

                if(sendModel)
                    glUniformMatrix4fv(uniforms[model], 1, GL_FALSE, glm::value_ptr(m));
            }

            void setVP(glm::mat4 vp){
                glUniformMatrix4fv(uniforms[VP], 1, GL_FALSE, glm::value_ptr(vp));
            }

            void setParallaxUniforms(glm::vec3 arg_viewPos, float arg_minLayers, float arg_maxLayers, float arg_heightScale, int arg_repeatTexture){
                glUniform3f(uniforms[viewPos], arg_viewPos.x, arg_viewPos.y, arg_viewPos.z);
                glUniform1f(uniforms[minLayers], arg_minLayers);
                glUniform1f(uniforms[maxLayers], arg_maxLayers);
                glUniform1f(uniforms[heightScale], arg_heightScale);
                glUniform1i(uniforms[repeatTexture], arg_repeatTexture);
            }

            void reset(){
                TextureManager::emptyBuffer();
            }

            void setTexture(Texture& texture, std::uint_fast16_t flags){
                int slot = TextureManager::addToBuffer(texture);

                if(flags & DiffuseMapped)
                    glUniform1i(uniforms[colorTex], slot);
                if(flags & AlbedoMapped)
                    glUniform1i(uniforms[matTex], slot);
                if(flags & NormalMapped)
                    glUniform1i(uniforms[normTex], slot);
                if(flags & ParallaxMapped)
                    glUniform1i(uniforms[heightTex], slot);
            }

            void setMultipliers(glm::vec4 color, glm::vec3 linfo){
                glUniform4f(uniforms[colorMultiplier], color.x, color.y, color.z, color.w);
                glUniform3f(uniforms[materialMultiplier], linfo.x, linfo.y, linfo.z);
            }

            void bind() { shader.bind(); }
    };

    int UltimateShader::maxUniformMatArray = -1;

    const char * UltimateShader::vertex = R"====(
                                            layout(location = 0) in vec3 aPos;
                                            layout(location = 1) in vec3 aNormal;                                            
                                            layout(location = 2) in vec4 aDiffuse;
                                            layout(location = 3) in vec3 aLightInfo;

                                            #if defined(DIFFUSE_MAPPED) || defined(MATERIAL_MAPPED) || defined(PARALLAX_MAPPED) || defined(NORMAL_MAPPED)
                                                layout(location = 4) in vec2 aTexCoord;
                                            #endif

                                            #if defined(PARALLAX_MAPPED) || defined(NORMAL_MAPPED)
                                                layout(location = 5) in vec3 aTangent;
                                            #endif
                                            
                                            #ifdef ANIMATED
                                                layout(location = 6) in vec4 weights;
                                                layout(location = 7) in ivec4 indices;
                                            #endif

                                            #ifdef INSTANCED
                                                layout(location = 8) in mat4 instancedModel;
                                            #endif

                                            #if defined(INSTANCED) || defined(ANIMATED)
                                                uniform mat4 VP;
                                            #endif


                                            #ifndef NORMAL_MAPPED
                                                out vec3 Normal;
                                            #endif
                                            out vec4 Diffuse;
                                            out vec3 LightInfo;

                                            #if defined(DIFFUSE_MAPPED) || defined(MATERIAL_MAPPED) || defined(PARALLAX_MAPPED) || defined(NORMAL_MAPPED)
                                                out vec2 texCoord;
                                            #endif

                                            #if defined(PARALLAX_MAPPED) || defined(NORMAL_MAPPED)
                                                out mat3 TBN;
                                            #endif

                                            #ifdef PARALLAX_MAPPED
                                                out vec3 tangentFragPos;
                                                out vec3 tangentViewPos;
                                            #endif


                                            #if (defined(PARALLAX_MAPPED) || defined(NORMAL_MAPPED) || defined(ANIMATED) ) && !defined(INSTANCED)
                                                uniform mat4 u_model;
                                            #endif

                                            #ifdef PARALLAX_MAPPED
                                                uniform vec3 viewPos;
                                            #endif
                                            
                                            #ifndef INSTANCED
                                                uniform mat3 TIModel;  //mat3(transpose(inverse(model)))
                                            #endif

                                            
                                            #if defined(ANIMATED) && !defined(INSTANCED)
                                                uniform mat4 bones[SKELETON_MAX];
                                            #elif defined(ANIMATED) && defined(INSTANCED)
                                                layout (std140) uniform SkeletonBlock
                                                {   
                                                    mat4 bone_buffer[SKELETON_BUFFER_MAX];
                                                };  

                                                uniform int skeleton_size;
                                            #endif
                                            

                                            uniform mat4 MVP;

                                            void main()
                                            {
                                                int i;

                                                
                                                #if defined(ANIMATED) && !defined(INSTANCED) 
                                                    mat4 boneTransform = mat4(0.0);

                                                    float allW = 0.0;
                                                    for(i = 0; i < 4; i++)
                                                    {
                                                        if(indices[i] == -1) break;
                                                        else boneTransform += bones[indices[i]] *  weights[i];
                                                        allW += weights[i];
                                                    }

                                                    boneTransform += mat4(1.0) * (1.0 - allW);

                                                #elif defined(ANIMATED) && defined(INSTANCED)
                                                    mat4 boneTransform = mat4(0.0);

                                                    float allW = 0.0;
                                                    for(i = 0; i < 4; i++)
                                                    {
                                                        if(indices[i] == -1) break;
                                                        else boneTransform += bone_buffer[gl_InstanceID * skeleton_size + indices[i]] * weights[i];
                                                        allW += weights[i];
                                                    }

                                                    boneTransform += mat4(1.0) * (1.0 - allW);
                                                #endif
                                                

                                                #ifdef INSTANCED
                                                    mat4 model = instancedModel;
                                                #elif defined(PARALLAX_MAPPED) || defined(NORMAL_MAPPED) || defined(ANIMATED)
                                                    mat4 model = u_model;
                                                #endif

                                                
                                                #ifdef ANIMATED
                                                    model = model * boneTransform; //apply bone transform
                                                #endif
                                                


                                                #if !defined(INSTANCED) && !defined(ANIMATED) 
                                                        mat3 NModel = TIModel;  //use transposed inverted matrix, which is calculated on the CPU and sent as a uniform
                                                #else
                                                    #ifdef FORCE_PRECISION
                                                        mat3 NModel = mat3(transpose(inverse(model))); // calculate transposed inverted matrix
                                                    #else
                                                        mat3 NModel = mat3(model); // aproximate transposed inverted matrix
                                                    #endif
                                                #endif

                                                Diffuse = aDiffuse;
                                                LightInfo = aLightInfo;

                                                #ifndef NORMAL_MAPPED
                                                    Normal = normalize(NModel * aNormal);
                                                #endif


                                                #if defined(DIFFUSE_MAPPED) || defined(MATERIAL_MAPPED) || defined(PARALLAX_MAPPED) || defined(NORMAL_MAPPED)
                                                    texCoord = aTexCoord;
                                                #endif



                                                #if defined(PARALLAX_MAPPED) || defined(NORMAL_MAPPED)                                                
                                                    vec3 T = normalize(NModel * aTangent);
                                                    vec3 N = normalize(NModel * aNormal);

                                                    T = normalize(T - dot(T, N) * N); //reorthogonalization
                                                    vec3 B = cross(N, T);

                                                    TBN = mat3(T, B, N);
                                                #endif


                                                #ifdef PARALLAX_MAPPED
                                                    mat3 TTBN = transpose(TBN);
                                                    vec3 fragPos = vec3(model * vec4(aPos, 1.0));

                                                    tangentViewPos  = TTBN * viewPos;
                                                    tangentFragPos  = TTBN * fragPos;
                                                #endif

                                                #if defined(INSTANCED) ||  defined(ANIMATED)
                                                    gl_Position = VP * model * vec4(aPos, 1.0);
                                                #else
                                                    gl_Position = MVP * vec4(aPos, 1.0);
                                                #endif
                                            }


                                            )====";

    const char * UltimateShader::fragment = R"====(
                                            #ifndef NORMAL_MAPPED
                                                in vec3 Normal;
                                            #endif
                                            in vec4 Diffuse;
                                            in vec3 LightInfo;

                                            #if defined(DIFFUSE_MAPPED) || defined(MATERIAL_MAPPED) || defined(PARALLAX_MAPPED) || defined(NORMAL_MAPPED)
                                                in vec2 texCoord;
                                            #endif

                                            #if defined(PARALLAX_MAPPED) || defined(NORMAL_MAPPED)
                                                in mat3 TBN;
                                            #endif

                                            #ifdef PARALLAX_MAPPED
                                                in vec3 tangentFragPos;
                                                in vec3 tangentViewPos;
                                            #endif


                                            #ifdef DIFFUSE_MAPPED
                                                uniform sampler2D colorTex;
                                            #endif

                                            #ifdef MATERIAL_MAPPED
                                                uniform sampler2D matTex;
                                            #endif

                                            #ifdef NORMAL_MAPPED
                                                uniform sampler2D normTex;
                                            #endif

                                            #ifdef PARALLAX_MAPPED
                                                uniform sampler2D heightTex;

                                                uniform float minLayers;
                                                uniform float maxLayers;
                                                uniform float heightScale;
                                                uniform bool repeatTexture;
                                            #endif

                                            #ifdef MULTIPLY
                                                uniform vec4 colorMultiplier;
                                                uniform vec3 materialMultiplier;
                                            #endif


                                            layout(location = 0) out vec4 out_color;
                                            #ifndef AETHER
                                                layout(location = 1) out vec4 out_material;
                                                layout(location = 2) out vec4 out_normal;
                                            #endif


                                            #ifdef PARALLAX_MAPPED

                                                /* learnopengl.com : CC BY-NC 4.0 license */
                                                vec2 ParallaxMapping(vec2 texCoords, vec3 tangentViewDir)
                                                { 
                                                    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), tangentViewDir)));  
                                                    // calculate the size of each layer
                                                    float layerDepth = 1.0 / numLayers;
                                                    // depth of current layer
                                                    float currentLayerDepth = 0.0;
                                                    // the amount to shift the texture coordinates per layer (from vector P)
                                                    vec2 P = tangentViewDir.xy / tangentViewDir.z * heightScale; 
                                                    vec2 deltaTexCoords = P / numLayers;
                                                
                                                    // get initial values
                                                    vec2  currentTexCoords     = texCoords;
                                                    float currentDepthMapValue = texture(heightTex, currentTexCoords).x;
                                                    
                                                    while(currentLayerDepth < currentDepthMapValue)
                                                    {
                                                        // shift texture coordinates along direction of P
                                                        currentTexCoords -= deltaTexCoords;
                                                        // get depthmap value at current texture coordinates
                                                        currentDepthMapValue = texture(heightTex, currentTexCoords).x;  
                                                        // get depth of next layer
                                                        currentLayerDepth += layerDepth;  
                                                    }
                                                    
                                                    // get texture coordinates before collision (reverse operations)
                                                    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

                                                    // get depth after and before collision for linear interpolation
                                                    float afterDepth  = currentDepthMapValue - currentLayerDepth;
                                                    float beforeDepth = texture(heightTex, prevTexCoords).x - currentLayerDepth + layerDepth;
                                                
                                                    // interpolation of texture coordinates
                                                    float weight = afterDepth / (afterDepth - beforeDepth);
                                                    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

                                                    return finalTexCoords;
                                                }
                                                
                                            #endif

                                            void main()
                                            {
                                                vec3 normal;
                                                vec3 material = vec3(0.0);
                                                vec4 color = vec4(0.0);

                                                #ifdef PARALLAX_MAPPED
                                                    vec2 coord = ParallaxMapping(texCoord, normalize(tangentViewPos - tangentFragPos));
                                                    if((coord.x < 0.0 || coord.x > 1.0 || coord.y < 0.0 || coord.y > 1.0) && !repeatTexture)
                                                        discard;
                                                #elif defined(DIFFUSE_MAPPED) || defined(MATERIAL_MAPPED) || defined(NORMAL_MAPPED)
                                                    vec2 coord = texCoord;
                                                #endif

                                                #ifndef NORMAL_MAPPED
                                                    normal = Normal;
                                                #endif
                                                
                                                #ifdef COLORED
                                                    color += Diffuse;
                                                    material += LightInfo;
                                                #endif

                                                #ifdef DIFFUSE_MAPPED
                                                    color += texture(colorTex, coord);
                                                #endif

                                                #ifdef MATERIAL_MAPPED
                                                    material += texture(matTex, coord).xyz;
                                                #endif

                                                #ifdef NORMAL_MAPPED
                                                    normal = texture(normTex, coord).xyz;
                                                    normal = normalize(normal * 2.0 - 1.0);   
                                                    normal = normalize(TBN * normal); 
                                                #endif

                                                #if !defined(DIFFUSE_MAPPED) && !defined(COLORED)
                                                    color = vec4(0.7, 0.1, 0.1, 1.0);
                                                #endif

                                                #if !defined(MATERIAL_MAPPED) && !defined(COLORED)
                                                    material = vec3(0.3);
                                                #endif

                                                #ifdef MULTIPLY
                                                    color = color * colorMultiplier;
                                                    material =  material * materialMultiplier;
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
                                                    if (color.w < threshold[x % 4][y % 4])
                                                        discard;
                                                #endif

                                                out_color    = color;                               //color
                                                #ifndef AETHER
                                                    out_material = vec4(material, 1.0);             //material
                                                    out_normal   = vec4(normal * 0.5 + 0.5, 1.0);   //normal
                                                #endif
                                            }


                                            )====";

}

#endif