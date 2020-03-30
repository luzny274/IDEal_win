#ifndef Properties_BU
#define Properties_BU

#include "Core.hpp"

namespace ulm{
    class Properties{
        public:
            static Program * current;
            static bool change;
            
            static Program * onStart();
            static void handleError(String error);

            static String storagePath;

            static void runProgram(Program * program){
                current = program;
                change = true;
            }

            static String getResourcePath(const char * path){
                return(storagePath + BU_APP_NAME + "_resources/" + path);
            }
            static String getResourcePath(String path){
                return getResourcePath(path.getPtr());
            }

            static void handleError(const char * error){
                handleError(String(error));
            }

    };

    bool Properties::change = false;
    Program * Properties::current = NULL;
    String Properties::storagePath = "";
}

#endif