#ifndef BU_LIGHT_MANAGER_SHADERS_SPOT
#define BU_LIGHT_MANAGER_SHADERS_SPOT

namespace ulm{

    class SpotL_Shader{
        private:
            static String vertex;
            static String fragment;

            static String shadowVertex;
            static String shadowFragment;


            static String shadowMapVertex;
            static String shadowMapFragment;

            int maxCount = 2;

        public:
            Shader shader;
            Shader shadowing;
            Shader shadowMapping;

            enum UNIFORM_LOCATIONS{
                inverseView = 0,
                inverseProjection = 1,
                depthTex = 2,
                colorTex = 3,
                lightInfoTex = 4,
                normalTex = 5,
                SpotLightBox_positions = 6,
                SpotLight_direction = 7,
                SpotLight_cutOff= 8,
                SpotLight_position = 9,
                SpotLight_color = 10,
                SpotLight_ambient = 11,
                SpotLight_range = 12,
                SpotLight_linear = 13,
                SpotLight_quadratic = 14,
                viewPos = 15,
                MVP = 16,
                right = 17,
                up = 18,
                U_Size = 19
            };

            Array<String> uniform_names = {
                String("inverseView"),
                String("inverseProjection"),
                String("depthTex"),
                String("colorTex"),
                String("lightInfoTex"),
                String("normalTex"),
                String("SpotLightBox_positions[0]"),
                String("SpotLight_direction[0]"),
                String("SpotLight_cutOff[0]"),
                String("SpotLight_position[0]"),
                String("SpotLight_color[0]"),
                String("SpotLight_ambient[0]"),
                String("SpotLight_range[0]"),
                String("SpotLight_linear[0]"),
                String("SpotLight_quadratic[0]"),
                String("viewPos"),
                String("MVP"),
                String("right"),
                String("up")
            };

            int uniforms[U_Size];
            Array<float> lightData[8];

            int shadow_uniforms[U_Size];

            int SM_MVP;

            int lightMVP_U;
            int quality_U;
            int bias_U;
            int shadowMap_U;

            void initialize(){
                compileSMShader();
                compileShadowShader();
                compile();
            }

            void compileSMShader(){
                shadowMapping.initialize(IncludeGLSL::begin + shadowMapVertex, IncludeGLSL::begin + shadowMapFragment);
                shadowMapping.bind();

                SM_MVP = glGetUniformLocation(shadowMapping.programID, "MVP");
            }

            void compileShadowShader(){
                shadowing.initialize(IncludeGLSL::begin + shadowVertex, IncludeGLSL::begin + shadowFragment);
                shadowing.bind();

                for(int i = 0; i < U_Size; i++) shadow_uniforms[i] = glGetUniformLocation(shadowing.programID, uniform_names[i].getPtr());

                //for(int i = 0; i < U_Size; i++) if(shadow_uniforms[i] == -1) ("\nERROR::UNIFORM::%s\n", uniform_names[i].getPtr());

                lightMVP_U = glGetUniformLocation(shadowing.programID, "lightMVP");
                quality_U = glGetUniformLocation(shadowing.programID, "quality");
                bias_U = glGetUniformLocation(shadowing.programID, "bias");
                shadowMap_U = glGetUniformLocation(shadowing.programID, "shadowMap");
            }

            void setSS_Uniforms(glm::mat4 lightVP, float quality, float bias, Texture& tex){
                glUniformMatrix4fv(lightMVP_U, 1, GL_FALSE, glm::value_ptr(lightVP));
                glUniform1f(quality_U, quality);
                glUniform1f(bias_U, bias);

                int slot = TextureManager::use(tex);
                glUniform1i(shadowMap_U, slot);
            }

            void setSM_MVP(const glm::mat4& matrix){
                glUniformMatrix4fv(SM_MVP, 1, GL_FALSE, glm::value_ptr(matrix));
            }

            void compile(){

                String beginning = IncludeGLSL::begin + "\n#define SPOT_MAX " + String(maxCount) + "\n";
                shader.initialize(beginning + vertex, beginning + fragment);
                shader.bind();
                for(int i = 0; i < U_Size; i++) uniforms[i] = glGetUniformLocation(shader.programID, uniform_names[i].getPtr());

                //for(int i = 0; i < U_Size; i++) if(uniforms[i] == -1) ("\nERROR::UNIFORM::%s\n", uniform_names[i].getPtr());
            }

            void setSpotLights(Array<SpotLight>& spotLights, bool shadowed){

                int * cUniforms;
                if(!shadowed) cUniforms = uniforms;
                else cUniforms = shadow_uniforms;

                int spotCount = spotLights.size;

                if(spotCount >= maxCount){
                    while(spotCount >= maxCount)
                        maxCount *= 2;
                    compile();
                }

                for(int i = 0; i < 8; i++) lightData[i].empty(lightData[i].size);


                /* Spot lights */

                for(SpotLight& l : spotLights){
                    glm::vec3 pos = l.getPosition();
                    glm::vec3 dir = l.getDirection();

                    /*glm::vec3 points[5];
                    glm::vec3 pos2 = pos + dir * l.range;
                    float r = l.range / glm::tan(l.angle / 2.0);

                    points[0] = pos;
                    points[1] = pos2 + up * r + right * r;
                    points[2] = pos2 - up * r + right * r;
                    points[3] = pos2 + up * r - right * r;
                    points[4] = pos2 - up * r - right * r;//*/

                    lightData[0].add(pos.x);
                    lightData[0].add(pos.y);
                    lightData[0].add(pos.z);
                    lightData[1].add(l.color.x);
                    lightData[1].add(l.color.y);
                    lightData[1].add(l.color.z);
                    lightData[2].add(dir.x);
                    lightData[2].add(dir.y);
                    lightData[2].add(dir.z);
                    lightData[3].add(glm::cos(l.angle));
                    lightData[4].add(l.ambient);
                    lightData[5].add(l.range);
                    
                    lightData[6].add(l.linear);
                    lightData[7].add(l.quadratic);
                }

                if(spotCount > 0){
                    glUniform3fv(cUniforms[SpotLight_position], spotCount, lightData[0].getPtr());
                    glUniform3fv(cUniforms[SpotLight_color], spotCount, lightData[1].getPtr());
                    glUniform3fv(cUniforms[SpotLight_direction], spotCount, lightData[2].getPtr());
                    glUniform1fv(cUniforms[SpotLight_cutOff], spotCount, lightData[3].getPtr());
                    glUniform1fv(cUniforms[SpotLight_ambient], spotCount, lightData[4].getPtr());
                    glUniform1fv(cUniforms[SpotLight_range], spotCount, lightData[5].getPtr());
                    glUniform1fv(cUniforms[SpotLight_linear], spotCount, lightData[6].getPtr());
                    glUniform1fv(cUniforms[SpotLight_quadratic], spotCount, lightData[7].getPtr());
                }
            }

            void setGBuffer(int slot, bool shadowed){

                int * cUniforms;
                if(!shadowed) cUniforms = uniforms;
                else cUniforms = shadow_uniforms;

                glUniform1i(cUniforms[depthTex],       slot);
                glUniform1i(cUniforms[colorTex],       slot + 1);
                glUniform1i(cUniforms[lightInfoTex],   slot + 2);
                glUniform1i(cUniforms[normalTex],      slot + 3);
            }

            void setCamera(Camera& camera, bool shadowed){

                int * cUniforms;
                if(!shadowed) cUniforms = uniforms;
                else cUniforms = shadow_uniforms;

                glm::mat4 invView = glm::inverse(camera.getView());
                glm::mat4 invProj = glm::inverse(camera.projection);

                glUniformMatrix4fv(cUniforms[inverseView], 1, GL_FALSE, glm::value_ptr(invView));
                glUniformMatrix4fv(cUniforms[inverseProjection], 1, GL_FALSE, glm::value_ptr(invProj));

                glm::vec3 viewPosition = camera.position;
                glUniform3f(cUniforms[viewPos], viewPosition.x, viewPosition.y, viewPosition.z);


                glm::vec3 camRight = camera.getRight();
                glUniform3f(cUniforms[right], camRight.x, camRight.y, camRight.z);

                glm::vec3 camUp = camera.getUp();
                glUniform3f(cUniforms[up], camUp.x, camUp.y, camUp.z);

                glm::mat4 VP = camera.getVP();
                glUniformMatrix4fv(cUniforms[MVP], 1, GL_FALSE, glm::value_ptr(VP));
            }
    };

    /* Normal */

    String SpotL_Shader::vertex = R"====(
                                            layout(location = 0) in vec2 aPos;

                                            //uniform vec3 SpotLightBox_positions[SPOT_MAX * 4];
                                            uniform vec3  SpotLight_position[SPOT_MAX];    
                                            uniform float SpotLight_range[SPOT_MAX];

                                            uniform vec3 viewPos;
                                            uniform mat4 MVP;

                                            uniform vec3 right;
                                            uniform vec3 up;

                                            out vec2 coord;
                                            flat out int instanceID;

                                            void main(){
                                                instanceID = gl_InstanceID;

                                                vec3 pos_world = vec3(0.0);
                                                vec4 pos = vec4(0.0);

                                                if(length(viewPos - SpotLight_position[gl_InstanceID]) >= SpotLight_range[gl_InstanceID]){
                                                    pos_world = SpotLight_position[gl_InstanceID] +  
                                                                    right * aPos.x * SpotLight_range[gl_InstanceID] + 
                                                                    up * aPos.y * SpotLight_range[gl_InstanceID];

                                                    pos = MVP * vec4(pos_world, 1.0);
                                                }else{
                                                    pos.xy = aPos.xy;
                                                    pos.w = 1.0;
                                                }
                                                
                                                coord = (pos.xy / pos.w) * 0.5 + 0.5;
                                                gl_Position = pos;
                                            }
                                            
                                        )====";


    String SpotL_Shader::fragment = IncludeGLSL::spotLightCalculation + IncludeGLSL::posFromDepth + R"====(
                                            in vec2 coord;
                                            flat in int instanceID;

                                            //G-Buffer
                                            uniform sampler2D depthTex;
                                            uniform sampler2D colorTex;
                                            uniform sampler2D lightInfoTex; // specular, shininess, emission
                                            uniform sampler2D normalTex;

                                            uniform mat4 inverseProjection;
                                            uniform mat4 inverseView;

                                            uniform vec3 viewPos;

                                            uniform int spotCount;

                                            uniform vec3  SpotLight_position[SPOT_MAX];    
                                            uniform vec3  SpotLight_color[SPOT_MAX];
                                            uniform vec3  SpotLight_direction[SPOT_MAX];
                                            uniform float SpotLight_cutOff[SPOT_MAX];
                                            uniform float SpotLight_ambient[SPOT_MAX];
                                            uniform float SpotLight_range[SPOT_MAX];
                                            uniform float SpotLight_linear[SPOT_MAX];
                                            uniform float SpotLight_quadratic[SPOT_MAX];

                                            layout( location = 0 ) out vec4 out_color;

                                            void main(){
                                                
                                                vec3 fragPos = posFromDepth(texture(depthTex, coord).x, inverseProjection, inverseView, coord);
                                                vec3 diffuse = texture(colorTex, coord).xyz;
                                                vec3 lightInfo = texture(lightInfoTex, coord).xyz;
                                                vec3 normal = texture(normalTex, coord).xyz * 2.0 - 1.0;


                                                vec3 viewDirection = normalize(viewPos - fragPos);

                                                vec3 result = calSpotLight( SpotLight_position[instanceID], SpotLight_color[instanceID], SpotLight_direction[instanceID], SpotLight_cutOff[instanceID], SpotLight_ambient[instanceID], 
                                                                SpotLight_range[instanceID], SpotLight_linear[instanceID], SpotLight_quadratic[instanceID],
                                                                diffuse, lightInfo,
                                                                fragPos, viewPos, normal, 
                                                                0.0, viewDirection);

                                                out_color = vec4(result, 1.0);
                                            }
                                            
                                        )====";

    /* With Shadows */

    String SpotL_Shader::shadowVertex = R"====(
                                            #define SPOT_MAX 1

                                            layout(location = 0) in vec2 aPos;

                                            uniform vec3  SpotLight_position[SPOT_MAX];    
                                            uniform float SpotLight_range[SPOT_MAX];

                                            uniform vec3 viewPos;
                                            uniform mat4 MVP;

                                            uniform vec3 right;
                                            uniform vec3 up;

                                            out vec2 coord;

                                            void main(){

                                                vec3 pos_world = vec3(0.0);
                                                vec4 pos = vec4(0.0);

                                                if(length(viewPos - SpotLight_position[gl_InstanceID]) >= SpotLight_range[gl_InstanceID]){
                                                    pos_world = SpotLight_position[gl_InstanceID] +  
                                                                    right * aPos.x * SpotLight_range[gl_InstanceID] + 
                                                                    up * aPos.y * SpotLight_range[gl_InstanceID];

                                                    pos = MVP * vec4(pos_world, 1.0);
                                                }else{
                                                    pos.xy = aPos.xy;
                                                    pos.w = 1.0;
                                                }
                                                
                                                coord = (pos.xy / pos.w) * 0.5 + 0.5;
                                                gl_Position = pos;
                                            }
                                            
                                        )====";


    String SpotL_Shader::shadowFragment = IncludeGLSL::spotLightCalculation + IncludeGLSL::posFromDepth + R"====(
                                            
                                            #define SPOT_MAX 1

                                            in vec2 coord;

                                            //G-Buffer
                                            uniform sampler2D depthTex;
                                            uniform sampler2D colorTex;
                                            uniform sampler2D lightInfoTex; // specular, shininess, emission
                                            uniform sampler2D normalTex;

                                            uniform mat4 inverseProjection;
                                            uniform mat4 inverseView;

                                            uniform vec3 viewPos;

                                            uniform int spotCount;

                                            uniform vec3  SpotLight_position[SPOT_MAX];    
                                            uniform vec3  SpotLight_color[SPOT_MAX];
                                            uniform vec3  SpotLight_direction[SPOT_MAX];
                                            uniform float SpotLight_cutOff[SPOT_MAX];
                                            uniform float SpotLight_ambient[SPOT_MAX];
                                            uniform float SpotLight_range[SPOT_MAX];
                                            uniform float SpotLight_linear[SPOT_MAX];
                                            uniform float SpotLight_quadratic[SPOT_MAX];

                                            uniform mat4 lightMVP; //90
                                            uniform float quality;
                                            uniform float bias;
                                            uniform sampler2D shadowMap;

                                            layout( location = 0 ) out vec4 out_color;

                                            float calculateShadow(vec3 fragPos){
                                                vec4 FragPosLightSpace = lightMVP * vec4(fragPos, 1.0);

                                                vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
                                                
                                                projCoords = projCoords * 0.5 + 0.5;
                                                
                                                float closestDepth = texture(shadowMap, projCoords.xy * quality).r; 
                                                
                                                float currentDepth = projCoords.z;
                                                
                                                float shadow = (currentDepth - bias) > closestDepth  ? 1.0 : 0.0;

                                                return shadow;
                                            }


                                            void main(){
                                                int instanceID = 0;
                                                
                                                vec3 fragPos = posFromDepth(texture(depthTex, coord).x, inverseProjection, inverseView, coord);
                                                vec3 diffuse = texture(colorTex, coord).xyz;
                                                vec3 lightInfo = texture(lightInfoTex, coord).xyz;
                                                vec3 normal = texture(normalTex, coord).xyz * 2.0 - 1.0;


                                                vec3 viewDirection = normalize(viewPos - fragPos);

                                                float shadow = calculateShadow(fragPos);

                                                vec3 result = calSpotLight( SpotLight_position[instanceID], SpotLight_color[instanceID], SpotLight_direction[instanceID], SpotLight_cutOff[instanceID], SpotLight_ambient[instanceID], 
                                                                SpotLight_range[instanceID], SpotLight_linear[instanceID], SpotLight_quadratic[instanceID],
                                                                diffuse, lightInfo,
                                                                fragPos, viewPos, normal, 
                                                                shadow, viewDirection);

                                                out_color = vec4(result, 1.0);
                                            }
                                            
                                        )====";

    /* Shadow generation */
    
    String SpotL_Shader::shadowMapVertex = R"====(
                                                layout(location = 0) in vec3 aPos;

                                                uniform mat4 MVP;

                                                void main()
                                                {
                                                    gl_Position = MVP * vec4(aPos, 1.0);
                                                }
                                            )====";

                                    
    String SpotL_Shader::shadowMapFragment = R"====(
                                                void main()
                                                {
                                                    //gl_FragDepth = x;
                                                }
                                            )====";

}

#endif