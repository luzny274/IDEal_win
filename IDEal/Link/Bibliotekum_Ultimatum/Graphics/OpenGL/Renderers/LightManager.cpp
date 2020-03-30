#ifndef BU_LIGHT_MANAGER
#define BU_LIGHT_MANAGER

#include "../../../Base/base.hpp"
#include "../Base/OpenGL.h"
#include "../Tools/ToolsGL.hpp"
#include "Renderer3D.cpp"
#include "LightManager_Shaders/shaders.hpp"

namespace ulm{

    class LightManager{
        private:
            static VertexArray quad;

            static GBuffer * gbuffer;

            static Array<DirectionalLight> dirLights;
            static Array<PointLight> pointLights;
            static Array<SpotLight> spotLights;

            static Array<DirectionalLight> dirLightsWshadows;
            static Array<PointLight> pointLightsWshadows;
            static Array<SpotLight> spotLightsWshadows;

            static glm::mat4 view;
            static glm::mat4 projection;
            static glm::vec3 pos;

            static Camera camera;

            static DirL_Shader dirL_Shader;
            static PointL_Shader pointL_Shader;
            static SpotL_Shader spotL_Shader;

            static Array<VertexArray> vas;
            static Array<glm::mat4> modelMatrices;

            static bool initialized;


            static void drawShadowedSpotLight(SpotLight& l){
                gbuffer->outputBuffer.bind();

                glDisable(GL_DEPTH_TEST);

                glEnable(GL_BLEND);
                glBlendEquation(GL_FUNC_ADD);
                glBlendFunc(GL_ONE, GL_ONE);

                glEnable(GL_STENCIL_TEST);
                glStencilFunc(GL_EQUAL, 1, 0xFF);
                glStencilMask(0x00);

                TextureManager::emptyAll();
                TextureManager::addToBuffer(gbuffer->getDepthTexture().id);
                TextureManager::addToBuffer(gbuffer->getColorTexture().id);
                TextureManager::addToBuffer(gbuffer->getAlbedoTexture().id);
                TextureManager::addToBuffer(gbuffer->getNormalVecTexture().id);

                spotL_Shader.shadowing.bind();


                Array<SpotLight> light(1);
                light.add(l);
                
                spotL_Shader.setSpotLights(light, true);
                spotL_Shader.setCamera(camera, true);
                spotL_Shader.setGBuffer(0, true);
                spotL_Shader.setSS_Uniforms(l.getVP(), l.shadowQuality, l.bias, l.shadowMap.depthStencilTexture);

                quad.draw();

                gbuffer->outputBuffer.unbind();
            }


            static void drawDirLights(int slot){
                dirL_Shader.shader.bind();
                dirL_Shader.setDirLights(dirLights);
                dirL_Shader.setCamera(view, projection, pos);

                dirL_Shader.setGBuffer(slot);

                quad.draw();
            }

            static void drawPointLights(int slot){
                pointL_Shader.shader.bind();
                pointL_Shader.setPointLights(pointLights);
                pointL_Shader.setCamera(camera);

                pointL_Shader.setGBuffer(slot);


                quad.drawInstanced(pointLights.size);
            }
            
            static void drawSpotLights(int slot){
                spotL_Shader.shader.bind();
                spotL_Shader.setSpotLights(spotLights, false);
                spotL_Shader.setCamera(camera, false);
                spotL_Shader.setGBuffer(slot, false);


                quad.drawInstanced(spotLights.size);
            }

            

            static void updateShadowMap(SpotLight& l, float quality, float bias){
                glEnable(GL_DEPTH_TEST);                
                glDisable(GL_BLEND);
                glDisable(GL_STENCIL_TEST);
                glm::mat4 VP = l.getVP();



                glBindFramebuffer(GL_FRAMEBUFFER, l.shadowMap.id);
                glViewport(0, 0, (int)((float)l.shadowMap.width * quality), (int)((float)l.shadowMap.height * quality));
                
                //glViewport(0, 0, 256, 256);
                l.shadowMap.clear(glm::vec3(0.0), 1.0);
                
                spotL_Shader.shadowMapping.bind();
                for(int i = 0; i < vas.size; i++){
                    spotL_Shader.setSM_MVP(VP * modelMatrices[i]);
                    vas[i].draw();
                }

                l.shadowQuality = quality;
                l.bias = bias;
            }


        public:

            static void initialize(){
                if(!initialized){
                    quad.initializeBuffers();
                    ByteArray VB = ulm::ByteArray::parse({  -1.f,-1.f,
                                                            -1.f, 1.f,
                                                             1.f,-1.f,
                                                             1.f, 1.f});

                    Array<unsigned int> IB = {0, 1, 2, 1, 2, 3};
                    Map<int, Type> attr = Map<int, Type>({2}, FLOAT);
                    quad.setBuffers(IB, VB, attr);

                    dirL_Shader.initialize();
                    pointL_Shader.initialize();
                    spotL_Shader.initialize();
                    initialized = true;
                }
            }

            static void begin(Camera& arg_camera, GBuffer * arg_gbuffer){
                initialize();
                gbuffer = arg_gbuffer;
                camera = arg_camera;

                dirLights.empty(dirLights.size);
                pointLights.empty(pointLights.size);
                spotLights.empty(spotLights.size);

                dirLightsWshadows.empty(dirLightsWshadows.size);
                pointLightsWshadows.empty(pointLightsWshadows.size);
                spotLightsWshadows.empty(spotLightsWshadows.size);

                vas.empty(vas.size);
                modelMatrices.empty(modelMatrices.size);

                view = arg_camera.getView();
                projection = arg_camera.getProjection();
                pos = arg_camera.position;
            }

            static void submit(DirectionalLight l){ submit(l , false); }
            static void submit(DirectionalLight l, bool shadow){
                if(shadow){
                    dirLightsWshadows.add(l);
                }else{
                    dirLights.add(l);
                }
            }

            static void submit(Array<PointLight>& lights, bool shadow){ 
                for(PointLight& l : lights)
                    submit(l , shadow); 
            }

            static void submit(Array<PointLight>& lights){ 
                for(PointLight& l : lights)
                    submit(l, false); 
            }
            static void submit(PointLight l){ submit(l , false); }

            static void submit(PointLight l, bool shadow){
                if(shadow){
                    pointLightsWshadows.add(l);
                }else{
                    pointLights.add(l);
                }
            }

            static void submit(SpotLight l){ submit(l , false); }
            static void submit(SpotLight l, bool shadow){
                if(shadow){
                    drawShadowedSpotLight(l);
                }else{
                    spotLights.add(l);
                }
            }


            static void draw(){
                gbuffer->outputBuffer.bind();

                glDisable(GL_CULL_FACE); 
                 
                glDisable(GL_DEPTH_TEST);
                glDepthMask(GL_FALSE); 

                glEnable(GL_BLEND);
                glBlendEquation(GL_FUNC_ADD);
                glBlendFunc(GL_ONE, GL_ONE);

                glEnable(GL_STENCIL_TEST);
                glStencilFunc(GL_EQUAL, 1, 0xFF);
                glStencilMask(0x00);




                TextureManager::emptyAll();
                TextureManager::addToBuffer(gbuffer->getDepthTexture().id);
                TextureManager::addToBuffer(gbuffer->getColorTexture().id);
                TextureManager::addToBuffer(gbuffer->getAlbedoTexture().id);
                TextureManager::addToBuffer(gbuffer->getNormalVecTexture().id);


                drawDirLights(0);
                drawPointLights(0);
                drawSpotLights(0);

                gbuffer->outputBuffer.unbind();
            }

            static void initializeSpotShadowMap(FrameBuffer * fb, int maxSize){
                fb->initializeDepth(maxSize, maxSize);
            }
    };


    VertexArray                 LightManager::quad;
    GBuffer *                   LightManager::gbuffer = NULL;
    Array<DirectionalLight>     LightManager::dirLights;
    Array<PointLight>           LightManager::pointLights;
    Array<SpotLight>            LightManager::spotLights;
    Array<DirectionalLight>     LightManager::dirLightsWshadows;
    Array<PointLight>           LightManager::pointLightsWshadows;
    Array<SpotLight>            LightManager::spotLightsWshadows;
    glm::mat4                   LightManager::view = glm::mat4(1.0);
    glm::mat4                   LightManager::projection = glm::mat4(1.0);
    glm::vec3                   LightManager::pos = glm::vec3(0.0);
    Camera                      LightManager::camera;
    DirL_Shader                 LightManager::dirL_Shader;
    PointL_Shader               LightManager::pointL_Shader;
    SpotL_Shader                LightManager::spotL_Shader;
    Array<VertexArray>          LightManager::vas;
    Array<glm::mat4>            LightManager::modelMatrices;
    bool                        LightManager::initialized = false;


}


#endif