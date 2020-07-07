#ifndef Properties_BU
#define Properties_BU

#ifndef NOT_BU_PROJECT
    #include "Core.hpp"
#else
    #include "ProgramStructure.cpp"
#endif

namespace ulm{
    class Properties{
        public:
            static Program * current;
            static bool change;
            
            #ifndef NOT_BU_PROJECT
                static Program * onStart();
            #endif
            static void handleError(String error);

            static String storagePath;

            static void runProgram(Program * program){
                current = program;
                change = true;
            }

            #ifndef NOT_BU_PROJECT

            static String getResourcePath(const char * path){
                return(storagePath + BU_APP_NAME + "_resources/" + path);
            }

            static String getResourcePath(String path){
                return getResourcePath(path.getPtr());
            }

            #endif

            static void handleError(const char * error){
                handleError(String(error));
            }

    };

    bool Properties::change = false;
    Program * Properties::current = NULL;
    String Properties::storagePath = "";
}

#endif