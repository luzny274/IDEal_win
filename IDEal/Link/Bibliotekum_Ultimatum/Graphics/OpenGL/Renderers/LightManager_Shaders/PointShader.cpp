#ifndef BU_LIGHT_MANAGER_SHADERS_POINT
#define BU_LIGHT_MANAGER_SHADERS_POINT

namespace ulm{

    class PointL_Shader{
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
                right = 6,
                up = 7,
                PointLight_position = 8,
                PointLight_color = 9,
                PointLight_ambient = 10,
                PointLight_range = 11,
                PointLight_linear = 12,
                PointLight_quadratic = 13,
                viewPos = 14,
                MVP = 15,
                U_Size = 16
            };

            Array<String> uniform_names = {
                String("inverseView"),
                String("inverseProjection"),
                String("depthTex"),
                String("colorTex"),
                String("lightInfoTex"),
                String("normalTex"),
                String("right"),
                String("up"),
                String("PointLight_position[0]"),
                String("PointLight_color[0]"),
                String("PointLight_ambient[0]"),
                String("PointLight_range[0]"),
                String("PointLight_linear[0]"),
                String("PointLight_quadratic[0]"),
                String("viewPos"),
                String("MVP")
            };

            int uniforms[U_Size];
            Array<float> lightData[6];

            void initialize(){
                compile();
            }
            void compile(){

                String beginning = IncludeGLSL::begin + "\n#define POINT_MAX " + String(maxCount) + "\n";
                shader.initialize(beginning + vertex, beginning + fragment);
                shader.bind();
                for(int i = 0; i < U_Size; i++) uniforms[i] = glGetUniformLocation(shader.programID, uniform_names[i].getPtr());
            }

            void setPointLights(Array<PointLight>& pointLights){
                int pointCount = pointLights.size;

                if(pointCount >= maxCount){
                    while(pointCount >= maxCount)
                        maxCount *= 2;
                    compile();
                }

                for(int i = 0; i < 6; i++) lightData[i].empty(lightData[i].size);

                
                /* Point lights */
                for(PointLight& l : pointLights){
                    glm::vec3 pos = l.getPosition();
                    lightData[0].add(pos.x);
                    lightData[0].add(pos.y);
                    lightData[0].add(pos.z);
                    lightData[1].add(l.color.x);
                    lightData[1].add(l.color.y);
                    lightData[1].add(l.color.z);
                    lightData[2].add(l.ambient);
                    lightData[3].add(l.range);
                    lightData[4].add(l.linear);
                    lightData[5].add(l.quadratic);
                }
                if(pointCount > 0) {
                    glUniform3fv(uniforms[PointLight_position], pointCount, lightData[0].getPtr());
                    glUniform3fv(uniforms[PointLight_color], pointCount, lightData[1].getPtr());
                    glUniform1fv(uniforms[PointLight_ambient], pointCount, lightData[2].getPtr());
                    glUniform1fv(uniforms[PointLight_range], pointCount, lightData[3].getPtr());
                    glUniform1fv(uniforms[PointLight_linear], pointCount, lightData[4].getPtr());
                    glUniform1fv(uniforms[PointLight_quadratic], pointCount, lightData[5].getPtr());
                }
            }

            void setGBuffer(int slot){
                glUniform1i(uniforms[depthTex],       slot);
                glUniform1i(uniforms[colorTex],       slot + 1);
                glUniform1i(uniforms[lightInfoTex],   slot + 2);
                glUniform1i(uniforms[normalTex],      slot + 3);
            }

            void setCamera(Camera& camera){
                glm::mat4 invView = glm::inverse(camera.getView());
                glm::mat4 invProj = glm::inverse(camera.projection);

                glUniformMatrix4fv(uniforms[inverseView], 1, GL_FALSE, glm::value_ptr(invView));
                glUniformMatrix4fv(uniforms[inverseProjection], 1, GL_FALSE, glm::value_ptr(invProj));

                glm::vec3 viewPosition = camera.position;
                glUniform3f(uniforms[viewPos], viewPosition.x, viewPosition.y, viewPosition.z);


                glm::vec3 camRight = camera.getRight();
                glUniform3f(uniforms[right], camRight.x, camRight.y, camRight.z);

                glm::vec3 camUp = camera.getUp();
                glUniform3f(uniforms[up], camUp.x, camUp.y, camUp.z);

                glm::mat4 VP = camera.getVP();
                glUniformMatrix4fv(uniforms[MVP], 1, GL_FALSE, glm::value_ptr(VP));
            }
    };


    String PointL_Shader::vertex = R"====(
                                            layout(location = 0) in vec2 aPos;


                                            uniform vec3 PointLight_position[POINT_MAX];   
                                            uniform float PointLight_range[POINT_MAX]; 

                                            uniform vec3 viewPos;

                                            uniform vec3 right;
                                            uniform vec3 up;

                                            uniform mat4 MVP;

                                            out vec2 coord;
                                            flat out int instanceID;

                                            void main(){
                                                instanceID = gl_InstanceID;

                                                vec3 pos_world = vec3(0.0);
                                                vec4 pos = vec4(0.0);

                                                if(length(viewPos - PointLight_position[gl_InstanceID]) >= PointLight_range[gl_InstanceID]){
                                                    pos_world = PointLight_position[gl_InstanceID] +  
                                                                    right * aPos.x * PointLight_range[gl_InstanceID] + 
                                                                    up * aPos.y * PointLight_range[gl_InstanceID] ;//*/

                                                    pos = MVP * vec4(pos_world, 1.0);
                                                }else{
                                                    pos.xy = aPos.xy;
                                                    pos.w = 1.0;
                                                }
                                                
                                                coord = (pos.xy / pos.w) * 0.5 + 0.5;
                                                gl_Position = pos;
                                            }
                                            
                                        )====";


    String PointL_Shader::fragment = IncludeGLSL::pointLightCalculation + IncludeGLSL::posFromDepth + R"====(
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

                                            uniform vec3 PointLight_position[POINT_MAX];    
                                            uniform vec3 PointLight_color[POINT_MAX];
                                            uniform float PointLight_ambient[POINT_MAX];
                                            uniform float PointLight_range[POINT_MAX];
                                            uniform float PointLight_linear[POINT_MAX];
                                            uniform float PointLight_quadratic[POINT_MAX];

                                            layout(location = 0) out vec4 out_color;

                                            void main(){
                                                //vec2 coord = gl_FragCoord.xy * 0.5 + 0.5;

                                                vec3 fragPos = posFromDepth(texture(depthTex, coord).x, inverseProjection, inverseView, coord);
                                                vec3 diffuse = texture(colorTex, coord).xyz;
                                                vec3 lightInfo = texture(lightInfoTex, coord).xyz;
                                                vec3 normal = texture(normalTex, coord).xyz * 2.0 - 1.0;


                                                vec3 viewDirection = normalize(viewPos - fragPos); 


                                                vec3 result = calPointLight( PointLight_position[instanceID], PointLight_color[instanceID], PointLight_ambient[instanceID], 
                                                                PointLight_range[instanceID], PointLight_linear[instanceID], PointLight_quadratic[instanceID],
                                                                diffuse, lightInfo,
                                                                fragPos, viewPos, normal, viewDirection);//*/

                                                result = vec3(0.15);
                                                out_color = vec4(result, 1.0);
                                            }
                                            
                                        )====";

}

#endif