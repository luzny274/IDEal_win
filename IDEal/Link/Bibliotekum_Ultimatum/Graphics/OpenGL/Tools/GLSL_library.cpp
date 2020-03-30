#ifndef BU_GLSL_LIBRARY_N
#define BU_GLSL_LIBRARY_N

#include "ToolsGL.hpp"

namespace ulm{
    class IncludeGLSL{
        public:
            static String begin;

            static String dirLightCalculation;
            static String pointLightCalculation;
            static String spotLightCalculation;

            static String lightCalculations;

            static String posFromDepth;
    };

    #ifdef BU_MOBILE
    String IncludeGLSL::begin = R"====(
                                    #version 300 es

                                    precision mediump float;
                                    precision mediump int;

                                )====";
    #else
    String IncludeGLSL::begin = R"====(
                                    #version 330

                                    precision mediump float;
                                    precision mediump int;

                                )====";
    #endif


    String IncludeGLSL::dirLightCalculation =   R"====(

                                            vec3 calDirLight(   vec3 arg_lightDir, vec3 arg_lightColor, float arg_lightAmbient, 
                                                                vec3 arg_diffuseColor, vec3 arg_LInfo, 
                                                                vec3 arg_fragmentPosition, vec3 arg_viewPosition, vec3 arg_normalVec,
                                                                float shadow, vec3 arg_viewDirection)
                                            {
                                                float d = 0.1;
                                                if(length(arg_normalVec) < d)
                                                    arg_normalVec = arg_lightDir;

                                                vec3 ambient_part = arg_lightAmbient * arg_lightColor * arg_diffuseColor;
                                                arg_normalVec = normalize(arg_normalVec);
                                                float cal_diff = max(dot(arg_normalVec, arg_lightDir), 0.0);
                                                vec3 diffuse_part = arg_lightColor * cal_diff * arg_diffuseColor;  

                                                float cal_spec = 0.0;

                                                /* Blinn-Phong */
                                                vec3 halfwayDir = normalize(arg_lightDir + arg_viewDirection);  
                                                if(arg_LInfo.y != 0.0) cal_spec = pow(max(dot(arg_normalVec, halfwayDir), 0.0), arg_LInfo.y * 1024.0);

                                                /* Phong */
                                                /*vec3 cal_reflectDir = reflect(-arg_lightDir, arg_normalVec);  
                                                if(arg_LInfo.y != 0.0) cal_spec = pow(max(dot(arg_viewDirection, cal_reflectDir), 0.0), arg_LInfo.y * 512.0);*/

                                                
                                                vec3 specular_part = arg_lightColor * cal_spec * arg_LInfo.x;

                                                return(ambient_part + (1.0 - shadow) * (diffuse_part + specular_part));
                                            }
                                                )====";

    String IncludeGLSL::pointLightCalculation =   R"====(

                                            vec3 calPointLight( vec3 lightPosition, vec3  lightColor, float lightAmbient, float lightRange, float lightLinear, float lightQuadratic,
                                                                vec3 diffuseColor, vec3 LInfo,
                                                                vec3 fragmentPosition, vec3 viewPosition, vec3 normalVec, vec3 viewDirection)
                                                {
                                                vec3 lightDir = normalize(lightPosition - fragmentPosition);


                                                float d = 0.1;
                                                if(length(normalVec) < d)
                                                    normalVec = lightDir;

                                                normalVec = normalize(normalVec);

                                                float distance    = length(lightPosition - fragmentPosition);
                                                if(distance < lightRange){
                                                    float attenuation = (1.0 + lightLinear * distance + 
                                                                    lightQuadratic * (distance * distance));    

                                                    vec3 ambient = lightAmbient * lightColor * diffuseColor;

                                                    float diff = max(dot(normalVec, lightDir), 0.0);
                                                    vec3 diffuse = lightColor * diff * diffuseColor;  

                                                    float spec = 0.0;

                                                    /* Blinn-Phong */
                                                    vec3 halfwayDir = normalize(lightDir + viewDirection);  
                                                    if(LInfo.y != 0.0) spec = pow(max(dot(normalVec, halfwayDir), 0.0), LInfo.y * 1024.0);

                                                    /* Phong */
                                                    /*vec3 reflectDir = reflect(-lightDir, normalVec);  
                                                    if(LInfo.y != 0.0) spec = pow(max(dot(viewDirection, reflectDir), 0.0), LInfo.y * 512.0);//*/
                                                    
                                                    vec3 specular = lightColor * spec * LInfo.x;  

                                                    return((ambient + diffuse + specular) / attenuation);
                                                } else return(vec3(0.0));
                                            }

                                            )====";


    String IncludeGLSL::spotLightCalculation =   R"====(

                                            vec3 calSpotLight(vec3 lightPosition, vec3  lightColor, vec3  lightDirection, float lightCutOff, float lightAmbient, float lightRange, float lightLinear, float lightQuadratic,
                                                            vec3 diffuseColor, vec3 LInfo,
                                                            vec3 fragmentPosition, vec3 viewPosition, vec3 normalVec, 
                                                            float shadow, vec3 viewDirection)
                                                {
                                                vec3 pom = lightPosition - fragmentPosition;
                                                vec3 lightDir = normalize(pom);
                                                
                                                float d = 0.1;
                                                if(length(normalVec) < d)
                                                    normalVec = lightDir;

                                                normalVec = normalize(normalVec);

                                                float theta = dot(lightDir, normalize(-lightDirection));
                                                float distance = length(pom);

                                                if(theta > lightCutOff && distance < lightRange) 
                                                {
                                                    //vec3 viewDirection = normalize(viewPosition - fragmentPosition); 
                                                    float attenuation = (1.0 + lightLinear * distance + 
                                                                    lightQuadratic * (distance * distance));    

                                                    vec3 ambient = lightAmbient * lightColor * diffuseColor;

                                                    //vec3 lightDir = normalize(lightPosition - fragmentPosition);
                                                    float diff = max(dot(normalVec, lightDir), 0.0);
                                                    vec3 diffuse = lightColor * diff * diffuseColor;  

                                                    float spec = 0.0;

                                                    /* Blinn-Phong */
                                                    vec3 halfwayDir = normalize(lightDir + viewDirection);  
                                                    if(LInfo.y != 0.0) spec = pow(max(dot(normalVec, halfwayDir), 0.0), LInfo.y * 1024.0);

                                                    /* Phong */
                                                    /*vec3 reflectDir = reflect(-lightDir, normalVec);  
                                                    if(LInfo.y != 0.0) spec = pow(max(dot(viewDirection, reflectDir), 0.0), LInfo.y * 512.0);//*/
                                                    
                                                    vec3 specular = lightColor * spec * LInfo.x;  

                                                    return((ambient + (1.0 - shadow) * (diffuse + specular)) / attenuation);
                                                } else return(vec3(0.0));
                                            }
                                        )====";

    String IncludeGLSL::lightCalculations = dirLightCalculation + pointLightCalculation + spotLightCalculation;

    String IncludeGLSL::posFromDepth = R"====(

                                        vec3 posFromDepth(float depth, mat4 projMatrixInv, mat4 viewMatrixInv, vec2 TexCoord) {
                                            float z = depth * 2.0 - 1.0;

                                            vec4 clipSpacePosition = vec4(TexCoord * 2.0 - 1.0, z, 1.0);
                                            vec4 viewSpacePosition = projMatrixInv * clipSpacePosition;

                                            // Perspective division
                                            viewSpacePosition /= viewSpacePosition.w;

                                            vec4 worldSpacePosition = viewMatrixInv * viewSpacePosition;

                                            return worldSpacePosition.xyz;
                                        }
                                        )====";
}


#endif