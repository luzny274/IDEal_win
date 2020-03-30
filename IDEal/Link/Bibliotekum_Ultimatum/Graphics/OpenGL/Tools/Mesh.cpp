#ifndef BU_MESH
#define BU_MESH

#include "ToolsGL.hpp"

namespace ulm{

    class Material{
        public:
            glm::vec4 diffuse = glm::vec4(0.7, 0.7, 0.7, 1.0);
            glm::vec3 albedo = glm::vec3(0.5, 0.5, 0.1); // specular, gloss, emission

            Material(){}
            Material(glm::vec4 color, float specular, float gloss, float emission){
                diffuse = color;
                albedo = glm::vec3(specular, gloss, emission);
            }

            void setDiffuse  (glm::vec4 color)   { diffuse     = color;      }
            void setSpecular (float specular)    { albedo.x = specular;   }
            void setGloss    (float gloss)       { albedo.y = gloss;      }
            void setEmission (float emission)    { albedo.z = emission;   }

            glm::vec4 getDiffuse ()  { return diffuse;      }
            float     getSpecular()  { return albedo.x;  }
            float     getGloss   ()  { return albedo.y;  }
            float     getEmission()  { return albedo.z;  }
    };

    class MeshProperties{
        public:
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            ModelMatrixBuffer modelMatrixBuffer;

            Skeleton skeleton;
            SkeletonBuffer skeletonBuffer;

            Texture diffuseMap;
            Texture albedoMap;
            Texture normalMap;
            Texture heightMap;

            glm::vec4 diffuseMultiplier = glm::vec4(1.0f);
            glm::vec3 albedoMultiplier = glm::vec3(1.0f);

            unsigned int parallax_minLayers = 4;
            unsigned int parallax_maxLayers = 16;
            float parallax_heightScale = 0.05;
            bool parallax_repeatTexture = false;

            Array<glm::ivec2> LOD_locations; // glm::ivec2(startIndex, numberOfIndices)


            void copy(const MeshProperties& other){
                modelMatrix = other.modelMatrix;
                modelMatrixBuffer = other.modelMatrixBuffer;
                skeleton = other.skeleton;
                skeletonBuffer = other.skeletonBuffer;
                diffuseMap = other.diffuseMap;
                albedoMap = other.albedoMap;
                normalMap = other.normalMap;
                heightMap = other.heightMap;
                diffuseMultiplier = other.diffuseMultiplier;
                albedoMultiplier = other.albedoMultiplier;
                parallax_minLayers = other.parallax_minLayers;
                parallax_maxLayers = other.parallax_maxLayers;
                parallax_heightScale = other.parallax_heightScale;
                parallax_repeatTexture = other.parallax_repeatTexture;
                LOD_locations = other.LOD_locations;
            }

            MeshProperties(){}
            MeshProperties(const MeshProperties& other){ copy(other); }
            MeshProperties& operator=(const MeshProperties& other){
                copy(other);
                return(*this);
            }

    };


    class Mesh{
        public:
            String name;
            Array<glm::vec3> vertices;
            Array<glm::vec3> normals;
            Array<glm::vec2> texture_coordinates;
            Array<Material> materials;

            Array<glm::vec3> tangents;

            Array<glm::ivec4> joint_indices;
            Array<glm::vec4> joint_weights;

            Array<unsigned int> indices;

            VertexArray va;
            MeshProperties properties; 

            void loadFrom(const char * path);

            void loadFrom(String path){ loadFrom(path.getPtr()); };
            void loadFromResources(const char * path){ loadFrom(Properties::getResourcePath(path)); }
            void loadFromResources(String path){ loadFrom(Properties::getResourcePath(path)); }

            void copy(const Mesh& other){
                name = other.name; vertices = other.vertices; normals = other.normals; texture_coordinates = other.texture_coordinates;
                materials = other.materials; indices = other.indices;

                va = other.va;
                properties = other.properties;
            }

            Mesh(){}
            Mesh(const Mesh& other){
                copy(other);
            }
            Mesh& operator=(const Mesh& other){
                copy(other);
                return(*this);
            }

            void setMaterial(Material material){
                materials.empty(vertices.size);
                for(int i = 0; i < vertices.size; i++) 
                    materials.add(material);
            }

            void setParallaxRes(int minLayers, int maxLayers){
                if(minLayers > maxLayers)
                    Properties::handleError("ULM::WARNING::MESH::setParallaxRes::arg minLayers is greater than arg maxLayers");
                
                properties.parallax_minLayers = minLayers;
                properties.parallax_maxLayers = maxLayers;
            }

            void initialize(){
                if(materials.isEmpty())
                    for(int i = 0; i < vertices.size; i++)
                        materials.add(Material());
                
                if(joint_indices.isEmpty())
                    for(int i = 0; i < vertices.size; i++)
                        joint_indices.add(glm::ivec4(-1));

                    
                if(joint_weights.isEmpty())
                    for(int i = 0; i < vertices.size; i++)
                        joint_weights.add(glm::vec4(0.f));

                if(vertices.isEmpty())
                    Properties::handleError("ULM::WARNING::MESH::initialize::No vertices found");
                if(normals.isEmpty())
                    Properties::handleError("ULM::WARNING::MESH::initialize::No normals found");
                if(materials.isEmpty())
                    Properties::handleError("ULM::WARNING::MESH::initialize::No materials found");
                if(texture_coordinates.isEmpty())
                    Properties::handleError("ULM::WARNING::MESH::initialize::No texture coordinates found");

                if(tangents.isEmpty())
                    calcTangents();

                va.initializeBuffers();

                ByteArray VB;
                
                if(vertices.size != normals.size || vertices.size != materials.size || vertices.size != texture_coordinates.size || vertices.size != joint_indices.size || vertices.size != joint_weights.size)
                    Properties::handleError("ULM::WARNING::MESH::initialize::Attributes have different counts");

                for(int i = 0; i < vertices.size; i++){
                    VB.add(vertices[i].x);
                    VB.add(vertices[i].y);
                    VB.add(vertices[i].z);

                    VB.add(normals[i].x);
                    VB.add(normals[i].y);
                    VB.add(normals[i].z);

                    VB.add(materials[i].diffuse.x);
                    VB.add(materials[i].diffuse.y);
                    VB.add(materials[i].diffuse.z);
                    VB.add(materials[i].diffuse.w);
                    
                    VB.add(materials[i].albedo.x);
                    VB.add(materials[i].albedo.y);
                    VB.add(materials[i].albedo.z);

                    VB.add(texture_coordinates[i].x);
                    VB.add(texture_coordinates[i].y);

                    VB.add(tangents[i].x);
                    VB.add(tangents[i].y);
                    VB.add(tangents[i].z);

                    VB.add(joint_weights[i].x);
                    VB.add(joint_weights[i].y);
                    VB.add(joint_weights[i].z);
                    VB.add(joint_weights[i].w);
                    
                    VB.add(joint_indices[i].x);
                    VB.add(joint_indices[i].y);
                    VB.add(joint_indices[i].z);
                    VB.add(joint_indices[i].w);

                }

                Map<int, Type> attributes = Map<int, Type>({3, 3, 4, 3, 2, 3, 4}, FLOAT);
                attributes.add(Couple<int, Type>(4, INT));

                va.setBuffers(indices, VB, attributes);


                if(properties.LOD_locations.isEmpty())
                    properties.LOD_locations.add(glm::ivec2(0, indices.size));
            }

            void prepareFaceCulling(){
                for(int i = 0; i < indices.size; i+=3){
                    TriangleOrientation ori = Math::getTriangleOrientation(vertices[indices[i]], vertices[indices[i+1]], vertices[indices[i+2]], normals[indices[i]]);
                    if(ori != CounterClockwise){
                        Math::swap(&indices[i], &indices[i+1]);
                    }
                }
            }

            void calcTangents(){
                if(properties.LOD_locations.isEmpty())
                    properties.LOD_locations.add(glm::ivec2(0, indices.size));

                
                glm::vec3 triangle[3];
                glm::vec2 uvs[3];
                glm::vec3 tangent;
                tangents = Array<glm::vec3>(vertices.size, true);


                for(int i = 0; i < properties.LOD_locations[0].y; i+= 3){
                    triangle[0] = vertices[indices[i+0]];
                    triangle[1] = vertices[indices[i+1]];
                    triangle[2] = vertices[indices[i+2]];
                    uvs[0] = texture_coordinates[indices[i]];
                    uvs[1] = texture_coordinates[indices[i+1]];
                    uvs[2] = texture_coordinates[indices[i+2]];

                    glm::vec3 edge1 = triangle[1] - triangle[0];
                    glm::vec3 edge2 = triangle[2] - triangle[0];
                    glm::vec2 deltaUV1 = uvs[1] - uvs[0];
                    glm::vec2 deltaUV2 = uvs[2] - uvs[0];

                    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

                    tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
                    tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
                    tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
                    tangent = glm::normalize(tangent);

                    tangents[indices[i+0]] = tangent;
                    tangents[indices[i+1]] = tangent;
                    tangents[indices[i+2]] = tangent;
                }
            }

            void smoothNormals(float maxAngle){
                glm::vec3 newNormals[normals.size];


                for(int i = 0; i < indices.size; i++)
                {
                    newNormals[indices[i]] = normals[indices[i]];
                    int b = 0;
                    for(int ii = 0; ii < indices.size; ii++)
                    {
                        if(i != ii)
                            if(vertices[indices[i]] == vertices[indices[ii]])
                            {
                                b++;
                                float angle = Math::getAngle(normals[indices[i]], normals[indices[ii]]);
                                
                                if(angle <= maxAngle && angle > 0.0f)
                                {
                                    newNormals[indices[i]] += normals[indices[ii]];
                                }                       
                            }
                    }
                }

                for(int i = 0; i < indices.size; i++)
                    normals[indices[i]] = glm::normalize(newNormals[indices[i]]);
            }

            void halfEdgeCollapse(int numberOfLevels){
                if(properties.LOD_locations.isEmpty())
                    properties.LOD_locations.add(glm::ivec2(0, indices.size));

                float maxLen = 0.f;

                for(int i = 0; i < indices.size; i += 3){
                    float len1 = glm::length(vertices[indices[i + 0]] - vertices[indices[i + 1]]);
                    float len2 = glm::length(vertices[indices[i + 1]] - vertices[indices[i + 2]]);
                    float len3 = glm::length(vertices[indices[i + 2]] - vertices[indices[i + 1]]);
                    if(len1 > maxLen) maxLen = len1;
                    if(len2 > maxLen) maxLen = len2;
                    if(len3 > maxLen) maxLen = len3;
                }

                for(int level = 1; level <= numberOfLevels; level++){

                    float tresh = maxLen / (1.f * ((float)numberOfLevels + 1 - (float)level));
                    
                    Array<glm::vec3> del;
                    Array<int> rep;

                    for(int i = properties.LOD_locations[level - 1].x; i < properties.LOD_locations[level - 1].x + properties.LOD_locations[level - 1].y; i+=3)
                        for(int x = 0; x < 3; x++)
                            for(int y = 0; y < 3; y++)
                                if(x != y){
                                    int ind1 = indices[i + x];
                                    int ind2 = indices[i + y];

                                    if(del.contains(vertices[ind1])) ind1 = rep[del.indexOf(vertices[ind1])];
                                    if(del.contains(vertices[ind2])) ind2 = rep[del.indexOf(vertices[ind2])];//*/
                                    
                                    int pom = del.indexOf(vertices[ind1]);
                                    if(pom != -1)
                                        if(vertices[rep[pom]] == vertices[ind2])
                                            continue;

                                    pom = del.indexOf(vertices[ind2]);
                                    if(pom != -1)
                                        if(vertices[rep[pom]] == vertices[ind1])
                                            continue;
                                            

                                    glm::vec3 ver1 = vertices[ind1];
                                    glm::vec3 ver2 = vertices[ind2];

                                    float len = glm::length(ver1 - ver2);
                                    if(len < tresh){
                                        del.add(ver1);
                                        rep.add(ind2);
                                    }
                                }

                    Array<int> newIndices;

                    for(int i = properties.LOD_locations[level - 1].x; i < properties.LOD_locations[level - 1].x + properties.LOD_locations[level - 1].y; i += 3){
                        
                        int ind[3] = {(int)indices[i], (int)indices[i+1], (int)indices[i+2]};

                        for(int ii = 0; ii < 3; ii++)
                            if(del.contains(vertices[ind[ii]]))
                                ind[ii] = rep[del.indexOf(vertices[ind[ii]])];

                        if(vertices[ind[0]] == vertices[ind[1]] || vertices[ind[0]] == vertices[ind[2]] || vertices[ind[1]] == vertices[ind[2]])
                            continue;


                        for(int ii = 0; ii < 3; ii++)
                            newIndices.add(ind[ii]);
                    }
                    
                    for(int i = 0; i < newIndices.size; i += 3){
                        glm::vec3 triangle1[3] = {  vertices[newIndices[i + 0]],
                                                    vertices[newIndices[i + 1]],
                                                    vertices[newIndices[i + 2]]};
                        bool unique = true;

                        for(int ii = i + 3; ii < newIndices.size; ii += 3){                        
                            glm::vec3 triangle2[3] = {  vertices[newIndices[ii + 0]],
                                                        vertices[newIndices[ii + 1]],
                                                        vertices[newIndices[ii + 2]]};

                            if(Math::matchTriangles(triangle1, triangle2))
                                unique = false;
                        }

                        if(unique)
                            for(int ii = 0; ii < 3; ii++)
                                indices.add(newIndices[i + ii]);
                    }//*/
                    //level = h;
                    int start = properties.LOD_locations[level - 1].x + properties.LOD_locations[level - 1].y;
                    int size = indices.size;

                    for(int i = 0; i < properties.LOD_locations.size; i++)
                        size -=  properties.LOD_locations[i].y;
                    
                    properties.LOD_locations.add(glm::ivec2(start, size));
                        
                }
            }

            void loadResourcesTextures(const char * diffusePath, const char * albedoPath, const char * normalPath, const char * heightPath){
                if(diffusePath != NULL)     properties.diffuseMap   =   ulm::Texture(ulm::Properties::getResourcePath(diffusePath));
                if(albedoPath != NULL)      properties.albedoMap =   ulm::Texture(ulm::Properties::getResourcePath(albedoPath));
                if(normalPath != NULL)      properties.normalMap    =   ulm::Texture(ulm::Properties::getResourcePath(normalPath));
                if(heightPath != NULL)      properties.heightMap    =   ulm::Texture(ulm::Properties::getResourcePath(heightPath));
            }

            void createTestCube();

    };
    
    class IQE{
        private:
            static String readWord(FILE * f){
                char c;
                String word = "";

                int success;
                while(true){
                    success = fscanf(f, "%c", &c);

                    if(c != ' ' && c != '\n' && c != '\t' && c != '\"' && success != EOF)
                        word.append(c);
                    else
                        break;
                }
                    
                return(word);
            }

        public:
            static void load(Mesh * mesh, Array<Skeleton> * frames, const char * path){
                FILE * f = fopen(path, "r");
                if(f == NULL){
                    Properties::handleError("ULM::ERROR::IQE::LOAD::file not found");
                    return;
                }

                if(frames   != NULL)  *frames   = Array<Skeleton>();
                if(mesh     != NULL)  *mesh     = Mesh();


                Skeleton skeleton;

                int nv = 0;
                int vv = 0;

                char c;
                
                while(!feof(f)){
                    String word = readWord(f);

                    if(word == "mesh" && mesh != NULL)
                    {           
                        fscanf(f, "%c", &c); // quote
                        String name = readWord(f); //read name
                        mesh->name = name;                 
                    }
                    else if((word == "joint" || word == "oint"))
                    {                    
                        Joint joint;

                        fscanf(f, "%c", &c); // quote
                        String name = readWord(f); //read name
                        joint.name = name;
                        fscanf(f, " %d", &joint.parent); // parent index
                        while(c != ' ') fscanf(f, "%c", &c);
                        fscanf(f, "%f %f %f ", &joint.translation.x , &joint.translation.y, &joint.translation.z);
                        fscanf(f, "%f %f %f %f", &joint.rotation.x , &joint.rotation.y, &joint.rotation.z, &joint.rotation.w);
                        fscanf(f, "%c", &c);
                        if(c == ' ')
                            fscanf(f, "%f %f %f", &joint.scale.x , &joint.scale.y, &joint.scale.z);
                        else
                            joint.scale = glm::vec3(1.f);

                        skeleton.joints.add(joint);
                        if(mesh != NULL) mesh->properties.skeleton.joints.add(joint);                        
                    }
                    else if((word == "vp" || word == "p") && mesh != NULL)
                    {         
                        glm::vec3 pos;               
                        fscanf(f, "%f %f %f", &pos.x, &pos.y, &pos.z);
                        mesh->vertices.add(pos);
                        vv++;
                    }
                    else if((word == "vt" || word == "t") && mesh != NULL)
                    {         
                        glm::vec2 tex;               
                        fscanf(f, "%f %f", &tex.x, &tex.y);
                        mesh->texture_coordinates.add(tex);
                    }
                    else if((word == "vn" || word == "n") && mesh != NULL)
                    {         
                        glm::vec3 normal;               
                        fscanf(f, "%f %f %f", &normal.x, &normal.y, &normal.z);
                        mesh->normals.add(normal);

                        nv++;
                    }
                    else if((word == "vb" || word == "b") && mesh != NULL)
                    {         
                        glm::ivec4 indices = glm::ivec4(-1);
                        glm::vec4 weights = glm::vec4(0.f);
                        bool novyRadek = false;

                        c = '\0';
                        for(int i = 0; i < 4; i++){
                            if(c == '\n' || c == 'v') { 
                                novyRadek = true; 
                                break; 
                            }
                            fscanf(f, "%d %f", &indices[i], &weights[i]);
                            fscanf(f, "%c", &c);
                        }

                        while(c != '\n' && c != 'v' && !novyRadek)
                            fscanf(f, "%c", &c);

                        mesh->joint_indices.add(indices);
                        mesh->joint_weights.add(weights);

                    }
                    else if(word == "fm" && mesh != NULL)
                    {         
                        glm::ivec3 indices;
                        fscanf(f, "%d %d %d", &indices.x, &indices.y, &indices.z);
                        for(int i = 0; i < 3; i++) mesh->indices.add(indices[i]);
                    }
                    else if(word == "frame" && frames != NULL)
                    {         
                        Skeleton rig = skeleton;
                        for(int i = 0; i < rig.joints.size; i++){
                            while(c != ' ') fscanf(f, "%c", &c);
                            
                            fscanf(f, "%f %f %f ",     &rig.joints[i].translation.x    , &rig.joints[i].translation.y  , &rig.joints[i].translation.z);
                            fscanf(f, "%f %f %f %f",   &rig.joints[i].rotation.x       , &rig.joints[i].rotation.y     , &rig.joints[i].rotation.z     , &rig.joints[i].rotation.w);
                            fscanf(f, "%c", &c);
                            if(c == ' ')
                                fscanf(f, "%f %f %f",  &rig.joints[i].scale.x          , &rig.joints[i].scale.y        , &rig.joints[i].scale.z);
                            else
                                rig.joints[i].scale = glm::vec3(1.f);
                        }
                        frames->add(rig);
                    }
                }

                Array<glm::mat4> inv = skeleton.toMatrices();

                for(glm::mat4& mat : inv)
                    mat = glm::inverse(mat);

                if(mesh != NULL)
                    for(int i = 0; i < mesh->properties.skeleton.joints.size; i++)
                        mesh->properties.skeleton.joints[i].inverseDefault = inv[i];

                if(frames != NULL)
                    for(Skeleton& s : *frames)
                        for(int i = 0; i < s.joints.size; i++)
                            s.joints[i].inverseDefault = inv[i];

                mesh->prepareFaceCulling();

                fclose(f);
            }

            static void load(Mesh * mesh, Array<Skeleton> * frames, String path){ load(mesh, frames, path.getPtr()); }
    };

    void Mesh::createTestCube(){
        float cubeVertices[] = {
            0.5f, -0.5f, -0.5f, 
            -0.5f, -0.5f, -0.5f, 
            0.5f,  0.5f, -0.5f, 

            -0.5f,  0.5f, -0.5f, 
            0.5f,  0.5f, -0.5f, 
            -0.5f, -0.5f, -0.5f, 


            -0.5f, -0.5f,  0.5f, 
            0.5f, -0.5f,  0.5f, 
            0.5f,  0.5f,  0.5f, 

            0.5f,  0.5f,  0.5f, 
            -0.5f,  0.5f,  0.5f, 
            -0.5f, -0.5f,  0.5f, 


            -0.5f,  0.5f,  0.5f, 
            -0.5f,  0.5f, -0.5f, 
            -0.5f, -0.5f, -0.5f, 

            -0.5f, -0.5f, -0.5f, 
            -0.5f, -0.5f,  0.5f, 
            -0.5f,  0.5f,  0.5f, 


            0.5f,  0.5f, -0.5f, 
            0.5f,  0.5f,  0.5f, 
            0.5f, -0.5f, -0.5f, 

            0.5f, -0.5f,  0.5f, 
            0.5f, -0.5f, -0.5f, 
            0.5f,  0.5f,  0.5f, 


            -0.5f, -0.5f, -0.5f, 
            0.5f, -0.5f, -0.5f, 
            0.5f, -0.5f,  0.5f, 

            0.5f, -0.5f,  0.5f, 
            -0.5f, -0.5f,  0.5f, 
            -0.5f, -0.5f, -0.5f, 


            0.5f,  0.5f, -0.5f, 
            -0.5f,  0.5f, -0.5f, 
            0.5f,  0.5f,  0.5f, 
            
            -0.5f,  0.5f,  0.5f, 
            0.5f,  0.5f,  0.5f, 
            -0.5f,  0.5f, -0.5f
        };

        float texCoords[] = {
            1.f, 1.f,
            0.f, 1.f,
            1.f, 0.f,

            0.f, 0.f,
            1.f, 0.f,
            0.f, 1.f,

            0.f, 0.f,
            1.f, 0.f,            
            1.f, 1.f,

            1.f, 1.f,
            0.f, 1.f,
            0.f, 0.f,


            0.f, 0.f,
            1.f, 0.f,            
            1.f, 1.f,

            1.f, 1.f,
            0.f, 1.f,
            0.f, 0.f,


            1.f, 0.f,
            1.f, 1.f,
            0.f, 0.f,

            0.f, 1.f,
            0.f, 0.f,
            1.f, 1.f,


            0.f, 0.f,
            1.f, 0.f,
            1.f, 1.f,

            1.f, 1.f,
            0.f, 1.f,
            0.f, 0.f,


            0.f, 0.f,
            1.f, 0.f,
            0.f, 1.f,

            1.f, 1.f,
            0.f, 1.f,
            1.f, 0.f,

        };

        float cubeNormals[] = {
            0.0f,  0.0f, -1.0f,
            0.0f,  0.0f, -1.0f, 
            0.0f,  0.0f, -1.0f, 
            0.0f,  0.0f, -1.0f, 
            0.0f,  0.0f, -1.0f, 
            0.0f,  0.0f, -1.0f, 

            0.0f,  0.0f,  1.0f,
            0.0f,  0.0f,  1.0f,
            0.0f,  0.0f,  1.0f,
            0.0f,  0.0f,  1.0f,
            0.0f,  0.0f,  1.0f,
            0.0f,  0.0f,  1.0f,

            -1.0f,  0.0f,  0.0f,
            -1.0f,  0.0f,  0.0f,
            -1.0f,  0.0f,  0.0f,
            -1.0f,  0.0f,  0.0f,
            -1.0f,  0.0f,  0.0f,
            -1.0f,  0.0f,  0.0f,

            1.0f,  0.0f,  0.0f,
            1.0f,  0.0f,  0.0f,
            1.0f,  0.0f,  0.0f,
            1.0f,  0.0f,  0.0f,
            1.0f,  0.0f,  0.0f,
            1.0f,  0.0f,  0.0f,

            0.0f, -1.0f,  0.0f,
            0.0f, -1.0f,  0.0f,
            0.0f, -1.0f,  0.0f,
            0.0f, -1.0f,  0.0f,
            0.0f, -1.0f,  0.0f,
            0.0f, -1.0f,  0.0f,

            0.0f,  1.0f,  0.0f,
            0.0f,  1.0f,  0.0f,
            0.0f,  1.0f,  0.0f,
            0.0f,  1.0f,  0.0f,
            0.0f,  1.0f,  0.0f,
            0.0f,  1.0f,  0.0f
        };

        for(int i = 0; i < 108; i+=3){
            vertices.add(glm::vec3(cubeVertices[i], cubeVertices[i + 1], cubeVertices[i + 2]));
            normals.add(glm::vec3(cubeNormals[i], cubeNormals[i + 1], cubeNormals[i + 2]));
            materials.add(Material());

            indices.add((unsigned int)(i/3));
        }


        for(int i = 0; i < 72; i+=2){
            texture_coordinates.add(glm::vec2(texCoords[i], texCoords[i + 1]));
        }

    }

}

#endif