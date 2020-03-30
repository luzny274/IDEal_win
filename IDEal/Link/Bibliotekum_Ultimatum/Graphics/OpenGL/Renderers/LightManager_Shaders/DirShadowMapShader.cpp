#ifndef BU_LIGHT_MANAGER_SHADERS_DIRECTIONAL_SHADOW_MAP
#define BU_LIGHT_MANAGER_SHADERS_DIRECTIONAL_SHADOW_MAP

namespace ulm{

    class DirShadowMap_Shader{
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
    };



    String DirShadowMap_Shader::vertex = R"====(
                                            attribute vec2 aPos;

                                            varying vec2 coord;

                                            void main(){
                                                coord = aPos * 0.5 + 0.5;
                                                gl_Position = vec4(aPos, 0.0, 1.0);
                                            }
                                            
                                        )====";


    String DirShadowMap_Shader::fragment = IncludeGLSL::dirLightCalculation + IncludeGLSL::posFromDepth + R"====(
                                            varying vec2 coord;

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

                                            void main(){
                                                vec3 fragPos = posFromDepth(texture2D(depthTex, coord).x, inverseProjection, inverseView, coord);
                                                vec3 diffuse = texture2D(colorTex, coord).xyz;
                                                vec3 lightInfo = texture2D(lightInfoTex, coord).xyz;
                                                vec3 normal = texture2D(normalTex, coord).xyz * 2.0 - 1.0;

                                                vec3 result = diffuse * lightInfo.z;

                                                vec3 viewDirection = normalize(viewPos - fragPos); 

                                                for(int i = 0; i < dirCount; i++){
                                                    result += calDirLight(-DirLight_direction[i], DirLight_color[i], DirLight_ambient[i], 
                                                                            diffuse, lightInfo, 
                                                                            fragPos, viewPos , normal,
                                                                            0.0, viewDirection);
                                                }

                                                gl_FragColor = vec4(result, 1.0);
                                            }
                                            
                                        )====";


}

#endif