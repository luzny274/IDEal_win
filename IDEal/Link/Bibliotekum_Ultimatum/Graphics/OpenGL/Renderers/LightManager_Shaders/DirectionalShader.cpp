#ifndef BU_LIGHT_MANAGER_SHADERS_DIRECTIONAL
#define BU_LIGHT_MANAGER_SHADERS_DIRECTIONAL

namespace ulm{

    class DirL_Shader{
        private:
            static String vertex;
            static String fragment;
            int maxCount = 2;

        public:
            Shader shader;

            enum UNIFORM_LOCATIONS{
                inverseView = 0,
                inverseProjection = 1,
                depthTex = 2,
                colorTex = 3,
                lightInfoTex = 4,
                normalTex = 5,
                dirCount = 6,
                DirLight_direction = 7,
                DirLight_ambient = 8,
                DirLight_color = 9,
                viewPos = 10,
                U_Size = 11
            };

            Array<String> uniform_names = {
                String("inverseView"),
                String("inverseProjection"),
                String("depthTex"),
                String("colorTex"),
                String("lightInfoTex"),
                String("normalTex"),
                String("dirCount"),
                String("DirLight_direction[0]"),
                String("DirLight_ambient[0]"),
                String("DirLight_color[0]"),
                String("viewPos")
            };

            int uniforms[U_Size];
            Array<float> lightData[3];

            void initialize(){
                compile();
            }
            void compile(){
                shader.initialize(IncludeGLSL::begin + vertex, IncludeGLSL::begin + "\n#define MAX_COUNT " + String(maxCount) + "\n" + fragment);
                for(int i = 0; i < U_Size; i++) uniforms[i] = glGetUniformLocation(shader.programID, uniform_names[i].getPtr());
                shader.bind();
            }

            void setDirLights(Array<DirectionalLight> dirLights){
                int dirLCount = dirLights.size;

                if(dirLCount >= maxCount){
                    while(dirLCount >= maxCount)
                        maxCount *= 2;
                    compile();
                }

                glUniform1i(uniforms[dirCount], dirLCount);

                for(int i = 0; i < 3; i++) lightData[i].empty(lightData[i].size);

                for(DirectionalLight& l : dirLights){
                    glm::vec3 dir = l.getDirection();
                    lightData[0].add(dir.x);
                    lightData[0].add(dir.y);
                    lightData[0].add(dir.z);
                    lightData[1].add(l.color.x);
                    lightData[1].add(l.color.y);
                    lightData[1].add(l.color.z);
                    lightData[2].add(l.ambient);  
                }

                if(dirLCount > 0){
                    glUniform3fv(uniforms[DirLight_direction], dirLCount, lightData[0].getPtr());
                    glUniform3fv(uniforms[DirLight_color], dirLCount, lightData[1].getPtr());
                    glUniform1fv(uniforms[DirLight_ambient], dirLCount, lightData[2].getPtr());
                }
            }

            void setGBuffer(int slot){
                glUniform1i(uniforms[depthTex],       slot);
                glUniform1i(uniforms[colorTex],       slot + 1);
                glUniform1i(uniforms[lightInfoTex],   slot + 2);
                glUniform1i(uniforms[normalTex],      slot + 3);
            }

            void setCamera(glm::mat4 view, glm::mat4 projection, glm::vec3 viewPosition){
                glm::mat4 invView = glm::inverse(view);
                glm::mat4 invProj = glm::inverse(projection);

                glUniformMatrix4fv(uniforms[inverseView], 1, GL_FALSE, glm::value_ptr(invView));
                glUniformMatrix4fv(uniforms[inverseProjection], 1, GL_FALSE, glm::value_ptr(invProj));

                glUniform3f(uniforms[viewPos], viewPosition.x, viewPosition.y, viewPosition.z);
            }
    };


    String DirL_Shader::vertex = R"====(
                                            layout(location = 0) in vec2 aPos;

                                            out vec2 coord;

                                            void main(){
                                                coord = aPos * 0.5 + 0.5;
                                                gl_Position = vec4(aPos, 0.0, 1.0);
                                            }
                                            
                                        )====";


    String DirL_Shader::fragment = IncludeGLSL::dirLightCalculation + IncludeGLSL::posFromDepth + R"====(
                                            in vec2 coord;

                                            //G-Buffer
                                            uniform sampler2D depthTex;
                                            uniform sampler2D colorTex;
                                            uniform sampler2D lightInfoTex; // specular, shininess, emission
                                            uniform sampler2D normalTex;

                                            uniform mat4 inverseProjection;
                                            uniform mat4 inverseView;

                                            uniform vec3 viewPos;

                                            uniform int dirCount;
                                            uniform vec3 DirLight_direction[MAX_COUNT];
                                            uniform float DirLight_ambient[MAX_COUNT];
                                            uniform vec3 DirLight_color[MAX_COUNT];

                                            layout(location = 0) out vec4 out_color;

                                            void main(){
                                                vec3 fragPos = posFromDepth(texture(depthTex, coord).x, inverseProjection, inverseView, coord);
                                                vec3 diffuse = texture(colorTex, coord).xyz;
                                                vec3 lightInfo = texture(lightInfoTex, coord).xyz;
                                                vec3 normal = texture(normalTex, coord).xyz * 2.0 - 1.0;

                                                vec3 result = diffuse * lightInfo.z;

                                                vec3 viewDirection = normalize(viewPos - fragPos); 

                                                for(int i = 0; i < dirCount; i++){
                                                    result += calDirLight(-DirLight_direction[i], DirLight_color[i], DirLight_ambient[i], 
                                                                            diffuse, lightInfo, 
                                                                            fragPos, viewPos , normal,
                                                                            0.0, viewDirection);
                                                }
                                                
                                                out_color = vec4(result, 1.0);
                                            }
                                            
                                        )====";

}

#endif