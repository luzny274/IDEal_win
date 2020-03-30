#ifndef BU_SHADER
#define BU_SHADER


#include "ToolsGL.hpp"

namespace ulm{
    class Shader{
        private:
            bool initialized = false;

        public:
            unsigned int programID;

            Array<String> uniformNames;
            Array<int> uniformLocations;
            
            int getLocation(String name){
                int index = uniformNames.indexOf(name);

                if(index == -1){
                    int location;
                    location = glGetUniformLocation(programID, name.getPtr());

                    if(location == -1){
                        Properties::handleError(String("ULM::SHADER::WARNING::UNIFORM NAME::") + name + "::DOES NOT EXIST");
                        fflush(stdout);
                    }

                    uniformNames.add(name);
                    uniformLocations.add(location);
                    return(location);
                }else{
                    return(uniformLocations[index]);
                }
            }


            Shader(){}            

            #ifndef BU_MOBILE
                Shader(String vertex, String geometry, String fragment){ initialize(vertex, geometry, fragment); }
                Shader(const char * vertex, const char * geometry, const char * fragment){ initialize(vertex, geometry, fragment); }
                void initialize(String vertex, String geometry, String fragment){ initialize(vertex.getPtr(), geometry.getPtr(), fragment.getPtr()); }

                void initialize(const char * vertex, const char * geometry, const char * fragment){
                    if(initialized) glDeleteProgram(programID);
                    else initialized = true;
                    
                    programID = createShaderProgram(vertex, geometry, fragment);                
                }
            #endif
            
            Shader(String vertex, String fragment){ initialize(vertex, fragment); }
            Shader(const char * vertex, const char * fragment){ initialize(vertex, fragment); }
            void initialize(String vertex, String fragment){ initialize(vertex.getPtr(), fragment.getPtr()); }
            

            void initialize(const char * vertex, const char * fragment){
                if(initialized) glDeleteProgram(programID);
                else initialized = true;
                
                programID = createShaderProgram(vertex, fragment);       

                bind();         
            }
            
            void bind()     { glUseProgram(programID);    }
            void destroy()  { glDeleteProgram(programID); }

            void setTexture (const char * uniform, Texture& texture){   glUniform1i(getLocation(String(uniform)), TextureManager::use(texture));               }
            
            void setInt     (const char * uniform, int i)           {   glUniform1i(getLocation(String(uniform)), i);                                          }
            void setIVec2   (const char * uniform, glm::ivec2 vec)  {   glUniform2i(getLocation(String(uniform)), vec.x, vec.y);                               }
            void setIVec3   (const char * uniform, glm::ivec3 vec)  {   glUniform3i(getLocation(String(uniform)), vec.x, vec.y, vec.z);                        }
            void setIVec4   (const char * uniform, glm::ivec4 vec)  {   glUniform4i(getLocation(String(uniform)), vec.x, vec.y, vec.z, vec.w);                 }
    
            void setFloat   (const char * uniform, float f)         {   glUniform1f(getLocation(String(uniform)), f);                                          }
            void setVec2    (const char * uniform, glm::vec2 vec)   {   glUniform2f(getLocation(String(uniform)), vec.x, vec.y);                               }
            void setVec3    (const char * uniform, glm::vec3 vec)   {   glUniform3f(getLocation(String(uniform)), vec.x, vec.y, vec.z);                        }
            void setVec4    (const char * uniform, glm::vec4 vec)   {   glUniform4f(getLocation(String(uniform)), vec.x, vec.y, vec.z, vec.w);                 }
    
            void setMat2    (const char * uniform, glm::mat2 mat)   {   glUniformMatrix2fv(getLocation(String(uniform)), 1, GL_FALSE, glm::value_ptr(mat));    }
            void setMat3    (const char * uniform, glm::mat3 mat)   {   glUniformMatrix3fv(getLocation(String(uniform)), 1, GL_FALSE, glm::value_ptr(mat));    }
            void setMat4    (const char * uniform, glm::mat4 mat)   {   glUniformMatrix4fv(getLocation(String(uniform)), 1, GL_FALSE, glm::value_ptr(mat));    }
    
    
            void setInt     (String uniform, int i)                 {  setInt   (uniform.getPtr(), i);    }
            void setIVec2   (String uniform, glm::ivec2 vec)        {  setIVec2 (uniform.getPtr(), vec);  }
            void setIVec3   (String uniform, glm::ivec3 vec)        {  setIVec3 (uniform.getPtr(), vec);  }
            void setIVec4   (String uniform, glm::ivec4 vec)        {  setIVec4 (uniform.getPtr(), vec);  }

            void setFloat   (String uniform, float f)               {  setFloat (uniform.getPtr(), f);    }
            void setVec2    (String uniform, glm::vec2 vec)         {  setVec2  (uniform.getPtr(), vec);  }
            void setVec3    (String uniform, glm::vec3 vec)         {  setVec3  (uniform.getPtr(), vec);  }
            void setVec4    (String uniform, glm::vec4 vec)         {  setVec4  (uniform.getPtr(), vec);  }

            void setMat2    (String uniform, glm::mat2 mat)         {  setMat2  (uniform.getPtr(), mat);  }
            void setMat3    (String uniform, glm::mat3 mat)         {  setMat3  (uniform.getPtr(), mat);  }
            void setMat4    (String uniform, glm::mat4 mat)         {  setMat4  (uniform.getPtr(), mat);  }
        
    };
}

#endif