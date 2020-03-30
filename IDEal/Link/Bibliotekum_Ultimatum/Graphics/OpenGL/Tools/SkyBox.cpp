#ifndef BU_SKYBOX
#define BU_SKYBOX

#include "../Tools/ToolsGL.hpp"

namespace ulm{
    class SkyBox{
        public:
            unsigned int ID = 0;
            bool initialized = false;


            void load(Array<Image>& faces){
                if(initialized) end();
                else initialized = true;

                glGenTextures(1, &ID);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

                for (int i = 0; i < 6; i++)
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, faces[i].width, faces[i].height, 0, GL_RGB, GL_UNSIGNED_BYTE, faces[i].data);
                
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

                initialized = true;
            }

            void load(Array<String> paths){
                Array<Image> images(6);
                for(int i = 0; i < 6; i++)
                    images.add(Image(paths[i]));
                load(images);

                for(Image& image : images)
                    image.empty();
            }
            
            void load(Array<char*> paths){
                Array<Image> images(6);
                for(int i = 0; i < 6; i++)
                    images.add(Image(paths[i]));
                load(images);

                for(Image& image : images)
                    image.empty();
            }

            void load(const char * front, const char * back, const char * top, const char * down, const char * right, const char * left)
            {
                Array<Image> images(6);
                images.add(Image(front , false));
                images.add(Image(back  , false));
                images.add(Image(top   , false));
                images.add(Image(down  , false));
                images.add(Image(right , false));
                images.add(Image(left  , false));
                load(images);
                for(Image& image : images)
                    image.empty();
            }


            void loadFromResources(const char * front, const char * back, const char * top, const char * down, const char * right, const char * left)
            {
                Array<Image> images(6);
                images.add(Image(Properties::getResourcePath(front) , false));
                images.add(Image(Properties::getResourcePath(back ) , false));
                images.add(Image(Properties::getResourcePath(top  ) , false));
                images.add(Image(Properties::getResourcePath(down ) , false));
                images.add(Image(Properties::getResourcePath(right) , false));
                images.add(Image(Properties::getResourcePath(left ) , false));
                load(images);
                for(Image& image : images)
                    image.empty();
            }

            void bind(){
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
            }

            void end(){
                if(initialized) glDeleteTextures(1, &ID);
            }
    };
}

#endif