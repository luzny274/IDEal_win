#ifndef BU_ANDROID_JNI
#define BU_ANDROID_JNI

#include "Core.hpp"
#include <string.h>
#include <android/log.h>
#include <jni.h>


void setStoragePath(JNIEnv * env, jstring path){
    bool boo = true;
    const char * nativePath = env->GetStringUTFChars(path, (jboolean*)&boo);

    ulm::String storagePath(nativePath);
    ulm::Properties::storagePath = storagePath;
}



extern "C" {

#ifndef BU_ULM_ANDROID
    JNIEXPORT void JNICALL Java_org_Bibliotekum_1Ultimatum_app_BU_1Functions_setStoragePath(JNIEnv * env, jobject o, jstring path){
        setStoragePath(env, path);
    }
#else
    
/*JNIEXPORT void JNICALL Java_org_Bibliotekum_1Ultimatum_app_BU_1Activity_setStoragePath(JNIEnv * env, jobject o, jstring path){
        setStoragePath(env, path);
    }

    JNIEXPORT void JNICALL Java_org_Bibliotekum_1Ultimatum_app_BU_1Activity_onStart(JNIEnv * env, jobject o, jint width, jint height){
        
    }

    JNIEXPORT void JNICALL Java_org_Bibliotekum_1Ultimatum_app_BU_1Activity_onDraw(JNIEnv * env, jobject o){
        
    }
    JNIEXPORT void JNICALL Java_org_Bibliotekum_1Ultimatum_app_BU_1Functions_nativeOnCreate(JNIEnv * env, jobject o){
        
        //Cardboard_initializeAndroid(javaVm, o);
        ulm::Properties::handleError("\ntadyyy2");
    }//*/
#endif


}
#ifdef BU_VR
    #include "Cardboard/cardboard_jni.cpp"

    /*#include "CardboardVR.cpp"
    
    JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
        ulm::CardboardVR::javaVM = vm;
        ulm::Properties::handleError("\ntadyyy1");
        return JNI_VERSION_1_6;
    }

    JNIEXPORT void JNICALL Java_org_Bibliotekum_1Ultimatum_app_BU_1Functions_nativeOnCreate(JNIEnv * env, jobject o){
        ulm::CardboardVR::initialize(o);

        ulm::CardboardVR::initializeLensDistortion(1980, 1024);
        ulm::Properties::handleError("\ntadyyy2");
    }//*/


#endif



#endif