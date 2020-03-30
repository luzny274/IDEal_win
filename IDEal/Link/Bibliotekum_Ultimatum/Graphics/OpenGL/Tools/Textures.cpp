#ifndef BU_TEXTURE_MANAGER_TEXTURE_BLB
#define BU_TEXTURE_MANAGER_TEXTURE_BLB


#include "ToolsGL.hpp"

namespace ulm{
    enum Tex_Round{TEXTURE_ROUND_NEAREST = 0, TEXTURE_ROUND_UP = 1, TEXTURE_ROUND_DOWN = 2};
    enum Tex_Mode{NEAREST = 0, LINEAR = 1};

    class FrameBuffer;
    
    class Texture{
        public:
            Image image;

            glm::ivec2 originalSize = glm::ivec2(0, 0);

            int numberOfChannels = 0;
            bool initialized = false;
            unsigned int id = 0;

            Texture(){}

            Texture(const char * path){initialize(path, true);}
            Texture(const Image& arg_image){initialize(arg_image, true);}

            Texture(const char * path, bool freeMemory){initialize(path, freeMemory);}
            Texture(const Image& arg_image, bool freeMemory){initialize(arg_image, freeMemory);}

            void initialize(const char * path, bool freeMemory){ image.load(path); initialize(freeMemory); }
            void initialize(const Image& arg_image, bool freeMemory){ image = arg_image; initialize(freeMemory); }
            void initialize(bool freeMemory){initialize(false, TEXTURE_ROUND_UP, LINEAR, freeMemory);};


            Texture(const char * path, bool mipmapping, Tex_Round roundMode, Tex_Mode texMode, bool freeMemory){initialize(path, mipmapping, roundMode, texMode, freeMemory);}
            Texture(const Image& arg_image, bool mipmapping, Tex_Round roundMode, Tex_Mode texMode, bool freeMemory){initialize(arg_image, mipmapping, roundMode, texMode, freeMemory);}

            void initialize(const char * path, bool mipmapping, Tex_Round roundMode, Tex_Mode texMode, bool freeMemory){ image.load(path); initialize(mipmapping, roundMode, texMode, freeMemory); }
            void initialize(const Image& arg_image, bool mipmapping, Tex_Round roundMode, Tex_Mode texMode, bool freeMemory){ image = arg_image; initialize(mipmapping, roundMode, texMode, freeMemory); }
            void initialize(bool mipmapping, Tex_Round roundMode, Tex_Mode texMode, bool freeMemory);

            void round(Tex_Round roundMode){
                int newW = getNext2(image.width);
                int newH = getNext2(image.height);

                switch(roundMode){
                    case TEXTURE_ROUND_DOWN:
                        if(newW != image.width) newW /= 2;
                        if(newH != image.height) newH /= 2;
                        break;

                    case TEXTURE_ROUND_NEAREST:
                        if(abs(image.width - newW) > abs(image.width - (newW/2))) newW/=2;
                        if(abs(image.height - newH) > abs(image.height - (newH/2))) newH/=2;
                        break;

                    case TEXTURE_ROUND_UP:
                        break;
                }
                image.resize(newW, newH);
            }

            int getNext2(int puv){
                unsigned int v = (unsigned int)puv;

                v--;
                v |= v >> 1;
                v |= v >> 2;
                v |= v >> 4;
                v |= v >> 8;
                v |= v >> 16;
                v++;

                return((int)v);
            }

            void bind(){
                glBindTexture(GL_TEXTURE_2D, id);
            }

            void end(){
                if(initialized) glDeleteTextures(1, &id);
                id = 0;
                initialized = false;
                image.empty();
                originalSize = glm::ivec2(0, 0);
                numberOfChannels = 0;
            }

            void copy(const Texture& other){
                image = other.image;
                originalSize = other.originalSize;
                initialized = other.initialized;
                id = other.id;
            }

            void getFromFB(FrameBuffer frameBuffer);

            Texture(const Texture& other){
                copy(other);
            }

            Texture& operator=(const Texture& other){
                copy(other);
                return(*this);
            }
    };

    class TextureManager{
        private:
            static bool initialized;

        public:
            static int maxRes;
            static int maxTotalTextures;

            static int texBuffer;
            static int numberOfSlots;
            static unsigned int * textureIDs;

            static int current;

            static void initialize(){
                if(!initialized){
                    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxRes);
                    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTotalTextures);
                    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &numberOfSlots);

                    textureIDs = (unsigned int*)malloc(numberOfSlots * sizeof(unsigned int));
                    for(int i = 0; i < numberOfSlots; i++) textureIDs[i] = 0;
                    initialized = true;
                }
            }


            static void printInfo(){
                initialize();
                printf("\nmax resolution of texture: %d\n", maxRes);
                printf("number of total texture slots: %d\n", maxTotalTextures);
                printf("number of texture slots: %d\n", numberOfSlots);
                fflush(stdout);
            }

            static bool isBound(unsigned int id, int * slot){
                initialize();
                *slot = -1;
                for(int i = 0; i < numberOfSlots; i++){
                    if(textureIDs[i] == id){
                        *slot = i;
                        return(true);
                    }

                    if(textureIDs[i] == 0){
                        if(i > texBuffer) *slot = i;
                    }   
                } 
                if(*slot == -1) *slot = 0;
                
                return(false);
            }


            static void emptyAll(){
                initialize();
                emptyBuffer();
                for(int i = 0; i < numberOfSlots; i++){
                    if(textureIDs[i] != 0){
                        activateSlot(i);
                        textureIDs[i] = 0;
                        glBindTexture(GL_TEXTURE_2D, 0);
                    }
                }
            }

            static int emptyBuffer(){ 
                initialize();
                for(int i = 0; i <= texBuffer; i++){
                    if(textureIDs[i] != 0){
                        activateSlot(i);
                        textureIDs[i] = 0;
                        glBindTexture(GL_TEXTURE_2D, 0);
                    }
                }
                texBuffer = -1; 
                return(texBuffer);
            }

            static int addToBuffer(Texture& tex){ initialize(); return addToBuffer(tex.id); }
            static int addToBuffer(FrameBuffer& fb);
            static int addToBuffer(unsigned int texID){
                initialize();
                texBuffer++;
                if(textureIDs[texBuffer] != texID){
                    activateSlot(texBuffer);
                    textureIDs[texBuffer] = texID;
                    glBindTexture(GL_TEXTURE_2D, (unsigned int)texID);
                }
                return(texBuffer);
            }

            static int use(FrameBuffer& fb);
            static int use(Texture& texture){initialize(); return(use(texture.id)); }
            static int use(unsigned int texID){
                initialize();
                int slot;
                if(isBound(texID, &slot)){
                    activateSlot(slot);
                }else{
                    activateSlot(slot);
                    textureIDs[slot] = texID;
                    glBindTexture(GL_TEXTURE_2D, texID);
                }
                return(slot);
            }

            static void activateSlot(int slot){
                initialize();
                if(current != slot) glActiveTexture(GL_TEXTURE0 + slot);
                current = slot;
            }
            

            
    };

    bool            TextureManager::initialized = false;
    int             TextureManager::maxRes = 0;
    int             TextureManager::maxTotalTextures = 0;
    int             TextureManager::texBuffer = -1;
    int             TextureManager::numberOfSlots = 0;
    unsigned int *  TextureManager::textureIDs = NULL;
    int             TextureManager::current = -1;

    void Texture::initialize(bool mipmapping, Tex_Round roundMode, Tex_Mode texMode, bool freeMemory){
        if(!initialized) glGenTextures(1, &id);

        TextureManager::use((int)id);

        originalSize = glm::ivec2(image.width, image.height);

        /* OpenGL likes textures with sides with length power of two */
        round(roundMode);
        

        int numLevels = 1 + floor(log2((float)Math::max(image.width, image.height)));
        if(!mipmapping) numLevels = 1;
        Image mipmap = image;
        
        /* glGenerateMipmap(GL_TEXTURE_2D); */
        for (int i = 0; i < numLevels; i++) {
            if(mipmap.nrChannels == 4) glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, mipmap.width, mipmap.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, mipmap.data);
            if(mipmap.nrChannels == 3) glTexImage2D(GL_TEXTURE_2D, i, GL_RGB,  mipmap.width, mipmap.height, 0, GL_RGB,  GL_UNSIGNED_BYTE, mipmap.data);        
            if(mipmap.nrChannels == 2) glTexImage2D(GL_TEXTURE_2D, i, GL_RG,   mipmap.width, mipmap.height, 0, GL_RG,   GL_UNSIGNED_BYTE, mipmap.data);
            if(mipmap.nrChannels == 1) glTexImage2D(GL_TEXTURE_2D, i, GL_RED,  mipmap.width, mipmap.height, 0, GL_RED,  GL_UNSIGNED_BYTE, mipmap.data);
            
            mipmap.resize(Math::max(mipmap.width / 2, 1), Math::max(mipmap.height / 2, 1));            
        }
            
        if(texMode == LINEAR) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        if(mipmapping && texMode == LINEAR) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        else if(mipmapping && texMode == NEAREST) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        else if(!mipmapping && texMode == LINEAR) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        else if(!mipmapping && texMode == NEAREST) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        if(freeMemory) image.empty();
        initialized = true;

        numberOfChannels = image.nrChannels;
    }
}


#endif