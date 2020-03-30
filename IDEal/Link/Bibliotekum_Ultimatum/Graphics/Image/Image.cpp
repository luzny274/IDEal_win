#ifndef BU_IMAGE_CLASS_STB
#define BU_IMAGE_CLASS_STB

#include "../../Base/base.hpp"
#include "STB.h"
#include "../../Tools/String.cpp"

namespace ulm{
    class Image{
        public:
            int nrChannels = 0;
            int width = 0;
            int height = 0;
            unsigned char* data = NULL;

            Image(){}
            ~Image(){
                empty();
            }
            Image(const Image& other){
                copy(other);
            }
            Image(String path){
                load(path.getPtr());
            }
            Image(String path, bool flipVertically){
                load(path.getPtr(), flipVertically);
            }
            Image(const char * path){
                load(path);
            }
            Image(const char * path, bool flipVertically){
                load(path, flipVertically);
            }
            Image(int arg_chan, int arg_width, int arg_height){
                nrChannels = arg_chan;
                width = arg_width;
                height = arg_height;
                
                data = (unsigned char*)malloc(width * height * nrChannels * sizeof(unsigned char));
            }
            Image(int arg_chan, int arg_width, int arg_height, unsigned char * arg_data){
                nrChannels = arg_chan;
                width = arg_width;
                height = arg_height;
                
                data = (unsigned char*)malloc(width * height * nrChannels * sizeof(unsigned char));
                for(int i = 0; i < (width * height * nrChannels); i++) data[i] = arg_data[i];
            }

            void load(const char * path){
                empty();
                stbi_set_flip_vertically_on_load(true);
                data = stbi_load(path, &width, &height, &nrChannels, 0);
                if(data == NULL)
                    Properties::handleError(String("ULM::ERROR::IMAGE::Image not found"));
            }

            void load(const char * path, bool flipVertically){
                empty();
                stbi_set_flip_vertically_on_load(flipVertically);
                data = stbi_load(path, &width, &height, &nrChannels, 0);
                if(data == NULL)
                    Properties::handleError(String("ULM::ERROR::IMAGE::Image not found"));
            }

            void copy(const Image& other){
                empty();
                if(other.data != NULL){
                    width = other.width;
                    height = other.height;
                    nrChannels = other.nrChannels;

                    data = (unsigned char*)malloc(width * height * nrChannels * sizeof(unsigned char));
                    for(int i = 0; i < (width * height * nrChannels); i++) data[i] = other.data[i];
                }
            }

            void empty(){
                if(data != NULL) {
                    stbi_image_free(data);
                    data = NULL;
                    width = 0;
                    height = 0;
                    nrChannels = 0;
                }
            }

            unsigned char& get(int x, int y, int channel){
                return(data[y * width * nrChannels + x * nrChannels + channel]);
            }
            unsigned char* getPtr(){return(data);}

            void resize(int newWidth, int newHeight){
                if(data == NULL){
                    Properties::handleError(String("ULM::IMAGE::ERROR::RESIZING_NULL"));
                    fflush(stdout);
                }

                unsigned char * newdata;
                newdata = (unsigned char*) malloc(newWidth * newHeight * nrChannels);

                stbir_resize_uint8( data, width, height, 0,
                                    newdata, newWidth, newHeight, 0,
                                    nrChannels);
                                    
                stbi_image_free(data);
                data = newdata;
                width = newWidth;
                height = newHeight;
            }

            Image& operator=(const Image& other){
                copy(other);
                return(*this);
            }
            unsigned char& operator()(int x, int y, int channel){
                if(x >= width)              Properties::handleError(String("ULM::IMAGE::WARNING::X>=WIDTH::x=") + x + ",width=" + width);
                if(y >= height)             Properties::handleError(String("ULM::IMAGE::WARNING::Y>=HEIGHT::y=") + y + ",height=" + height);
                if(channel >= nrChannels)   Properties::handleError(String("ULM::IMAGE::WARNING::CHANNEL>=NRCHANNELS::channel=") + channel + "nrChannels=" + nrChannels);

                return(data[y * width * nrChannels + x * nrChannels + channel]);
            }
    };   
}


#endif