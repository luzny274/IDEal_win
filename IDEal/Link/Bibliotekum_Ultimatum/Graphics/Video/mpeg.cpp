#ifndef BU_MPEG_PROCESSER
#define BU_MPEG_PROCESSER

#include "../../Base/base.hpp"
#include "JO/jo_mpeg.cpp"

#define PL_MPEG_IMPLEMENTATION
#include "PL/pl_mpeg.h"

#include "../../Tools/String.cpp"
#include "../Image/Image.cpp"

namespace ulm{
    class MpegReader{
        private:
            plm_t * plm = NULL;
            plm_frame_t * nextFrame = NULL;

        public:
            int width = 0;
            int height = 0;
            double framerate = 0.0;
            double duration = 0.0;

            bool eof = false;

            MpegReader(String path){ open(path); }
            MpegReader(const char * path){ open(path); }
            void open(String path){ open(path.getPtr()); }

            void open(const char * path){
                plm = plm_create_with_filename(path);
                if (!plm) {
                    printf("Couldn't open file");
                    return;
                }

                plm_set_audio_enabled(plm, false, 0);

                duration = plm_get_time(plm);
                framerate = plm_get_framerate(plm);	
                width = plm_get_width(plm);
                height = plm_get_height(plm);

                eof = false;
            }

            Image getFrame(){
                Image image(3, width, height);

                if(nextFrame == NULL && !eof) nextFrame = plm_decode_video(plm);
                if(nextFrame != NULL) plm_frame_to_rgb(nextFrame, image.data);
                nextFrame = plm_decode_video(plm);
                if(nextFrame == NULL) eof = true;

                return image;
            }

            Array<Image> getVideo(){
                Array<Image> frames;
                while(!eof) frames.add(getFrame());
                return frames;
            }
    };

    class MpegWriter{
        private:
            FILE * file = NULL;

        public:

            MpegWriter(String path){ open(path); }
            MpegWriter(const char* path){ open(path); }

            void open(String path){ open(path.getPtr()); }
            void open(const char * path){ file = fopen(path, "w"); }

            void appendToAFile(const char * path){ file = fopen(path, "a"); }

            void write(Image frame, int frameRate){
                unsigned char * data = new unsigned char[frame.width * frame.height * 4];
                for(int i = 0; i < frame.width * frame.height; i++){
                    for(int c = 0; c < 4; c++){
                        if(frame.nrChannels > c) data[i * 4 + c] = frame.data[i * frame.nrChannels + c];
                        else data[i * 4 + c] = 0;
                    }
                }

                jo_write_mpeg(file, data, frame.width, frame.height, frameRate);

                delete[] data;
            }

            void flush(){
             	fclose(file);
                file = NULL;
            }
    };
}


#endif