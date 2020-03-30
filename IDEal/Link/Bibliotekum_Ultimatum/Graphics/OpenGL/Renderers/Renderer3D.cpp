#ifndef BU_SIMPLE_3D_RENDERER
#define BU_SIMPLE_3D_RENDERER

#include "../Tools/ToolsGL.hpp"
#include "GBuffer.cpp"
#include "ultimate3DShader.cpp"

namespace ulm{
    enum Cull{CullNone = 0, CullFrontFace = 1, CullBackFace = 2};


    class Renderer3D{
        private:
            static const int count = 2 * Aether;
            static UltimateShader shaders[count]; //Colored, Multiplied DiffuseMapped, AlbedoMapped, NormalMapped, ParallaxMapped

            static glm::mat4 VP;
            static glm::vec3 viewPos;

            static Array<VertexArray> vas[count];
            static Array<MeshProperties> props[count];
            static Array<unsigned int> LODs[count];

            static bool initialized;

            static void emptyArrays(){
                for(int i = 0; i < count; i++){
                    vas[i].empty(vas[i].size);
                    props[i].empty(props[i].size);
                    LODs[i].empty(LODs[i].size);
                }
            }

            static void draw(std::uint_fast16_t flags){

                if(vas[flags].size == 0)
                    return;

                UltimateShader * shader = &shaders[flags];

                if(!shader->initialized)
                    shader->initialize(flags);
                shader->bind();    

                for(int i = 0; i < vas[flags].size; i++){
                    MeshProperties * prop = &props[flags].get(i);
                    unsigned int LOD = LODs[flags].get(i);
                    
                    if((unsigned int) prop->LOD_locations.size <= LOD)
                        continue;
                    
                    if((flags & Animated) && (flags & Instanced) && !prop->skeletonBuffer.initialized)
                        Properties::handleError("ULM::ERROR::Renderer3D::draw::skeleton buffer is not initialized");
                    
                    if((flags & Instanced) && !prop->modelMatrixBuffer.initialized)
                        Properties::handleError("ULM::ERROR::Renderer3D::draw::model matrix buffer is not initialized");

                    if((flags & Animated) && (flags & Instanced))
                        shader->setSkeletonBuffer(prop->skeletonBuffer);


                    if(flags & Animated)
                        shader->setBones(prop->skeleton.toMatrices());//*/

                    if((flags & Instanced) || (flags & Animated))
                        shader->setVP(VP);
                    if(!(flags & Instanced) || (flags & Animated))
                        shader->setMVP(prop->modelMatrix, VP, (flags & ParallaxMapped) || (flags & NormalMapped) || (flags & Animated));

                    shader->reset();
                    if(flags & DiffuseMapped)   shader->setTexture(prop->diffuseMap,   DiffuseMapped);
                    if(flags & AlbedoMapped)  shader->setTexture(prop->albedoMap, AlbedoMapped);
                    if(flags & NormalMapped)    shader->setTexture(prop->normalMap,    NormalMapped);
                    if(flags & ParallaxMapped){
                        shader->setTexture(prop->heightMap, ParallaxMapped);
                        shader->setParallaxUniforms(viewPos, prop->parallax_minLayers, prop->parallax_maxLayers, prop->parallax_heightScale, prop->parallax_repeatTexture);
                    }
                    if(flags & Multiplied)
                        shader->setMultipliers(prop->diffuseMultiplier, prop->albedoMultiplier);

                    if((flags & Instanced) && (flags & Animated) && prop->skeletonBuffer.initialized){
                        prop->modelMatrixBuffer.bind();
                        if(prop->modelMatrixBuffer.size != prop->skeletonBuffer.size / prop->skeletonBuffer.skeletonSize)
                            Properties::handleError("ULM::ERROR::Renderer3D::draw::skeleton buffer and model matrix buffer have different sizes");
                        
                        vas[flags].get(i).drawInstanced((unsigned int) prop->LOD_locations[LOD].x, (unsigned int) prop->LOD_locations[LOD].y, Math::min(prop->modelMatrixBuffer.size, prop->skeletonBuffer.size / prop->skeletonBuffer.skeletonSize));
                    }
                    else if(flags & Instanced){
                        prop->modelMatrixBuffer.bind();
                        vas[flags].get(i).drawInstanced((unsigned int) prop->LOD_locations[LOD].x, (unsigned int) prop->LOD_locations[LOD].y, prop->modelMatrixBuffer.size);
                    }
                    else
                        vas[flags].get(i).draw((unsigned int) prop->LOD_locations[LOD].x, (unsigned int) prop->LOD_locations[LOD].y);
                }
            }

            static void submitMesh(Mesh& mesh, std::uint_fast16_t flags, unsigned int LOD){
                vas[flags].add(mesh.va);
                props[flags].add(mesh.properties);
                LODs[flags].add(LOD);
            }

            static void submitMesh(Mesh& mesh, std::uint_fast16_t flags){
                submitMesh(mesh, flags, 0);
            }

        public:
            static GBuffer * gbuffer;

            static void initialize(){
                /*if(!initialized)
                    for(std::uint_fast16_t flags = Animated; flags < count; flags++)
                        shaders[flags].initialize(flags);//*/
                initialized = true;
            }



            static void begin(Camera& camera, GBuffer * buffer){
                initialize();
                gbuffer = buffer;

                VP = camera.getVP();
                viewPos = camera.position;

                emptyArrays();
            }

            static void submit(Array<Mesh>& meshes, std::uint_fast16_t flags, unsigned int LOD){
                for(Mesh& mesh : meshes)
                    submitMesh(mesh, flags, LOD);
            }

            static void submit(Array<Mesh>& meshes, std::uint_fast16_t flags){
                for(Mesh& mesh : meshes)
                    submitMesh(mesh, flags);
            }

            static void submit(Mesh& mesh, std::uint_fast16_t flags, unsigned int LOD){
                submitMesh(mesh, flags, LOD);
            }

            static void submit(Mesh& mesh, std::uint_fast16_t flags){
                submitMesh(mesh, flags);
            }

            static void draw(Cull mode){
                gbuffer->bind();

                glEnable(GL_DEPTH_TEST);
                glDepthMask(GL_TRUE);
                glDepthFunc(GL_LESS);  

                glDisable(GL_BLEND);

                if(mode != CullNone) glEnable(GL_CULL_FACE);  
                else glDisable(GL_CULL_FACE);  
                glFrontFace(GL_CCW);
                
                if(mode == CullBackFace) glCullFace(GL_BACK);  
                else if(mode == CullFrontFace) glCullFace(GL_FRONT);  

                glEnable(GL_STENCIL_TEST);
                glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
                glStencilMask(0xFF);
                glStencilFunc(GL_ALWAYS, 1, 0xFF);


                for(std::uint_fast16_t i = 0; i < Aether; i++)
                    draw(i);

                gbuffer->unbind();
            }

            static void drawAether(Cull mode){
                gbuffer->outputBuffer.bind();

                glEnable(GL_DEPTH_TEST);
                glDepthMask(GL_FALSE);
                glDepthFunc(GL_LESS); 

                glEnable(GL_BLEND);
                glBlendEquation(GL_FUNC_ADD);
                glBlendFunc(GL_ONE, GL_ONE);

                if(mode != CullNone) glEnable(GL_CULL_FACE);   
                else glDisable(GL_CULL_FACE);  

                glFrontFace(GL_CCW);
                
                if(mode == CullBackFace) glCullFace(GL_BACK);  
                else if(mode == CullFrontFace) glCullFace(GL_FRONT);  

                glDisable(GL_STENCIL_TEST);
                glStencilMask(0x00);

                for(std::uint_fast16_t i = Aether; i < count; i++)
                    draw(i);

                gbuffer->outputBuffer.unbind();
            }

    };



    UltimateShader          Renderer3D::shaders[count];
    glm::mat4               Renderer3D::VP = glm::mat4(1.0f);
    glm::vec3               Renderer3D::viewPos;
    Array<VertexArray>      Renderer3D::vas[count];
    GBuffer *               Renderer3D::gbuffer = NULL;
    bool                    Renderer3D::initialized = false;
    Array<MeshProperties>   Renderer3D::props[count];
    Array<unsigned int>     Renderer3D::LODs[count];


}

#endif