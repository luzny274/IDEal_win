#ifndef IMPORTMODEL_BU
#define IMPORTMODEL_BU

#include "ToolsGL.hpp"

namespace ulm{

    class BlenderMesh{
        public:
            String name;
            Array<glm::vec3> vertices;
            Array<glm::vec3> normals;
            Array<glm::vec2> texture_coordinates;
            Array<glm::ivec3> indices;

            Material material;

            BlenderMesh(){}

            BlenderMesh(const BlenderMesh& other){
                name = other.name;
                vertices = other.vertices;
                normals = other.normals;
                texture_coordinates = other.texture_coordinates;
                indices = other.indices;
            }
            BlenderMesh& operator=(const BlenderMesh& other){
                name = other.name;
                vertices = other.vertices;
                normals = other.normals;
                texture_coordinates = other.texture_coordinates;
                indices = other.indices;

                return(*this);
            }

            BlenderMesh(String namearg){
                name = namearg;
            }


            void empty(){
                vertices.empty();
                normals.empty();
                texture_coordinates.empty();
                indices.empty();
            }

            

            void normalizeIndices(){
                int vmin  = indices[0].x;
                int vtmin = indices[0].y;
                int vnmin = indices[0].z;

                for(glm::ivec3 i : indices){
                    if(i.x < vmin)  vmin  = i.x;
                    if(i.y < vtmin) vtmin  = i.y;
                    if(i.z < vnmin) vnmin = i.z;
                }

                for(glm::ivec3& i : indices){
                    i.x -= vmin;
                    i.y -= vtmin;
                    i.z -= vnmin;
                }

            }

            Mesh convert(){
                Mesh mesh;
                mesh.name = name;

                for(int i = 0; i < indices.size; i++){
                    glm::vec3 vertex = vertices.array[indices.array[i].x];
                    glm::vec2 tex = texture_coordinates.array[indices.array[i].y];
                    glm::vec3 normal = normals.array[indices.array[i].z];



                    mesh.vertices.add(vertex);
                    mesh.texture_coordinates.add(tex);
                    mesh.normals.add(normal);
                    mesh.materials.add(Material());

                    mesh.indices.add((unsigned int)i);
                }

                return mesh;
            }
            
    };

    class Model{
        public:
            Array<Mesh> meshes;

            Model(){}
            Model(Array<Mesh> meshes_arg){
                meshes = meshes_arg;
            }
            Model(const char * path){
                loadFrom(path);
            }

            void loadFrom(const char * path){
                FILE * f = fopen(path, "r");

                if(f == NULL){
                    Properties::handleError("ULM::ERROR::MODEL::loadFrom::File not found");
                    return;
                }

                char znak;
                bool podm = true;

                bool first = true;
                Array<BlenderMesh> tmpmeshes;
                BlenderMesh current;

                while(podm){
                    if(fscanf(f, "%c", &znak) == EOF) podm = false;

                    if(znak == 'o'){
                        String jmeno;

                        fscanf(f, "%c", &znak);
                        fscanf(f, "%c", &znak);
                        while(znak != '\n'){
                            jmeno.append(znak);
                            fscanf(f, "%c", &znak);
                        }

                        if(!first) tmpmeshes.add(current);
                        else first = false;

                        current.empty();
                        current.name = jmeno;

                    }else if(znak == 'v'){
                        fscanf(f, "%c", &znak);

                        if(znak == ' '){
                            float v1;
                            float v2;
                            float v3;
                            fscanf(f, "%f %f %f", &v1, &v2, &v3);
                            current.vertices.add(glm::vec3(v1, v2, v3));

                        }else if(znak == 't'){
                            float t1;
                            float t2;
                            fscanf(f, " %f %f", &t1, &t2);
                            current.texture_coordinates.add(glm::vec2(t1, t2));

                        }else if(znak == 'n'){
                            float v1;
                            float v2;
                            float v3;
                            fscanf(f, " %f %f %f", &v1, &v2, &v3);
                            current.normals.add(glm::vec3(v1, v2, v3));
                        }
                        while(znak != '\n' && podm){
                            if(fscanf(f, "%c", &znak) == EOF) podm = false;
                        }

                    }else if(znak == 'f'){

                        for(int i = 0; i < 3; i++){
                            int i1;
                            int i2;
                            int i3;
                            fscanf(f, " %d/%d/%d", &i1, &i2, &i3);
                            current.indices.add(glm::ivec3(i1, i2, i3));

                        }
                        while(znak != '\n' && podm){
                            if(fscanf(f, "%c", &znak) == EOF) podm = false;
                        }

                    }else{
                        while(znak != '\n' && podm){
                            if(fscanf(f, "%c", &znak) == EOF) podm = false;
                        }
                    }
                }

                if(!first) tmpmeshes.add(current);

                for(BlenderMesh& m : tmpmeshes){
                    m.normalizeIndices();
                    meshes.add(m.convert());
                }


            }
    };

    void Mesh::loadFrom(const char * path){
        Model model(path);

        if(model.meshes.size > 0) *this = model.meshes[0];
        else Properties::handleError("ULM::ERROR::MESH::No meshes found in file");
    }


}

#endif