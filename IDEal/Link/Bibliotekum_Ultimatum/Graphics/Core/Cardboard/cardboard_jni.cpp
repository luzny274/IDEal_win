#ifndef BU_CARDBOARD_JNI
#define BU_CARDBOARD_JNI

#include <android/log.h>
#include <jni.h>
#include <EGL/egl.h>

#include <memory>

#include <chrono>

#include "vr/gvr/capi/include/gvr.h"
#include "vr/gvr/capi/include/gvr_audio.h"
#include "vr/gvr/capi/include/gvr_audio_surround.h"
#include "vr/gvr/capi/include/gvr_beta.h"
#include "vr/gvr/capi/include/gvr_controller.h"
#include "vr/gvr/capi/include/gvr_gesture.h"
#include "vr/gvr/capi/include/gvr_types.h"
#include "vr/gvr/capi/include/gvr_version.h"

#include "../Core.hpp"
#include "../../OpenGL/Renderers/Renderer2D.cpp"

#include "CardboardVR.cpp"

ulm::Program * currentProgram = NULL;
std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point last = std::chrono::steady_clock::now();
std::chrono::duration<float> deltaTime;
char emptyWchar[ULM_UTF_SIZE] = {0, 0, 0, 0};

int curW = 0;
int curH = 0;


ulm::FrameBuffer VR_fb_l;
ulm::FrameBuffer VR_fb_r;

bool BU_VR_initialized = false;

ulm::Sprite VRFullSprite;

void VR_initFB(){
  VR_fb_l.initializeColored(curW, curH, 4, true, false);
  VR_fb_r.initializeColored(curW, curH, 4, true, false);
}

ulm::Sprite createVRSprite(/*ulm::Eye eye*/){
    ulm::Sprite vr_sprite;

    vr_sprite.vertices.initialize(4);
    vr_sprite.colors.initialize(4);
    vr_sprite.texture_coordinates.initialize(4);

    float x = -1.f;
    float y = -1.f;
    float width = 2.f;
    float height = 2.f;

    vr_sprite.vertices.add(glm::vec2(x, y));
    vr_sprite.vertices.add(glm::vec2(x + width, y + height));
    vr_sprite.vertices.add(glm::vec2(x + width, y));
    vr_sprite.vertices.add(glm::vec2(x, y + height));

    vr_sprite.texture_coordinates.add(glm::vec2(0.0f, 0.0f));
    vr_sprite.texture_coordinates.add(glm::vec2(1.0f, 1.0f));
    vr_sprite.texture_coordinates.add(glm::vec2(1.0f, 0.0f));
    vr_sprite.texture_coordinates.add(glm::vec2(0.0f, 1.0f));

    /*if(eye == ulm::leftEye){
        vr_sprite.texture_coordinates.add(glm::vec2(0.0f, 0.0f));
        vr_sprite.texture_coordinates.add(glm::vec2(0.5f, 1.0f));
        vr_sprite.texture_coordinates.add(glm::vec2(0.5f, 0.0f));
        vr_sprite.texture_coordinates.add(glm::vec2(0.0f, 1.0f));
    }else{
        vr_sprite.texture_coordinates.add(glm::vec2(0.5f, 0.0f));
        vr_sprite.texture_coordinates.add(glm::vec2(1.0f, 1.0f));
        vr_sprite.texture_coordinates.add(glm::vec2(1.0f, 0.0f));
        vr_sprite.texture_coordinates.add(glm::vec2(0.5f, 1.0f));
    }//*/

    for(int i = 0; i < 4; i++) vr_sprite.colors.add(glm::vec4(1.f));
    vr_sprite.indices = {0, 1, 2, 0, 1, 3};
    vr_sprite.initialize(true);
    
    return(vr_sprite);
}

extern "C" {


JNIEXPORT void JNICALL Java_org_Bibliotekum_1Ultimatum_app_BU_1Functions_nativeOnDestroy(JNIEnv* env, jobject obj) { 
  //destroy
}

JNIEXPORT void JNICALL Java_org_Bibliotekum_1Ultimatum_app_BU_1Functions_nativeOnSurfaceCreated(JNIEnv* env, jobject obj) {
  //create opengl

  ulm::Window::mouse.relativeMode = true;
  currentProgram = ulm::Properties::onStart();


  glGenVertexArrays(1, &ulm::Window::vertexArray);
  glBindVertexArray(ulm::Window::vertexArray);

  /*leftSprite = createVRSprite(ulm::leftEye);
  rightSprite = createVRSprite(ulm::rightEye);//*/
  VRFullSprite = createVRSprite();
}

JNIEXPORT void JNICALL Java_org_Bibliotekum_1Ultimatum_app_BU_1Functions_nativeOnDrawEye(JNIEnv* env, jobject obj, 
          jfloat frontX, jfloat frontY, jfloat frontZ, 
					jfloat upX, jfloat upY, jfloat upZ, 
					jfloat rightX, jfloat rightY, jfloat rightZ,
					jfloatArray perspective, jint type) {


  jfloat* perspectivearr = env->GetFloatArrayElements(perspective, 0);
  float gvrPerspective[16];
  for(int i = 0; i < 16; i++)
    gvrPerspective[i] = perspectivearr[i];

  int fbo;
  glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &fbo);
  int vp [4]; 
  glGetIntegerv(GL_VIEWPORT, vp);

  ulm::DefaultFrameBuffer::id = fbo;
  ulm::DefaultFrameBuffer::x  = vp[0];
  ulm::DefaultFrameBuffer::y  = vp[1];
  ulm::DefaultFrameBuffer::w  = vp[2];
  ulm::DefaultFrameBuffer::h  = vp[3];

  ulm::Window::screenSize = glm::ivec2(vp[2], vp[3]);
  ulm::Window::width = vp[2];
  ulm::Window::height = vp[3];


  if(curW != vp[2] || curH != vp[3])
  {
    curW = vp[2];
    curH = vp[3];
    VR_initFB();
    currentProgram->resizeCallback(curW, curH);
  }

  ulm::CardboardVR::front = glm::vec3((float)frontX, (float)frontY, (float)frontZ);
  ulm::CardboardVR::up = glm::vec3((float)upX, (float)upY, (float)upZ);
  ulm::CardboardVR::right = glm::vec3((float)rightX, (float)rightY, (float)rightZ);
  ulm::CardboardVR::perspective = glm::make_mat4(gvrPerspective);

  ulm::DefaultFrameBuffer::bind();
  glBindVertexArray(ulm::Window::vertexArray);
  ulm::TextureManager::emptyAll();

  glClearColor(ulm::Window::clearColor.x, ulm::Window::clearColor.y, ulm::Window::clearColor.z, 1.00f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if((int)type == ulm::leftEye)
      VRFullSprite.texture = VR_fb_l.colorTextures[0];
  else if((int)type == ulm::rightEye)
      VRFullSprite.texture = VR_fb_r.colorTextures[0];

  ulm::Renderer2D::begin(glm::mat4(1.f));
  ulm::Renderer2D::draw(VRFullSprite, ulm::Textured);
  ulm::Renderer2D::end();

  BU_VR_initialized = true;
}


JNIEXPORT void JNICALL Java_org_Bibliotekum_1Ultimatum_app_BU_1Functions_nativeOnUpdate(JNIEnv* env, jobject obj,
          jboolean cardboard_trigger,
					jboolean isPlugged,
					jfloat lX, jfloat lY,
					jfloat rX, jfloat rY,
					jfloat triggerLeft, jfloat triggerRight,
					jboolean up,
					jboolean down,          
					jboolean left,          
					jboolean right,         
					jboolean start,         
					jboolean back,         
					jboolean leftShoulder,  
					jboolean rightShoulder, 
					jboolean A,
					jboolean B,       
					jboolean X,       
					jboolean Y)
{


  int fbo;
  glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &fbo);
  int vp [4]; 
  glGetIntegerv(GL_VIEWPORT, vp);

  int form_id = fbo;
  int form_x  = vp[0];
  int form_y  = vp[1];
  int form_w  = vp[2];
  int form_h  = vp[3];

  ulm::Window::mouse.relativeMode = true;

  if(ulm::Properties::change){
      ulm::Properties::change = false;
      delete currentProgram;
      currentProgram = ulm::Properties::current;
  }


  currentTime = std::chrono::steady_clock::now();
  deltaTime = currentTime - last;
  last = currentTime;

  for(int i = 0; i < ulm::SCANCODE_LCLICK; i++) ulm::Window::keysPressed[i] = 0;

  ulm::Window::keysPressed[ulm::SCANCODE_CARDBOARD_TRIGGER] = (bool)cardboard_trigger;
  memcpy(ulm::Window::textInput, emptyWchar, NK_UTF_SIZE);

  ulm::Window::mouse.x = 0;
  ulm::Window::mouse.y = 0;
  ulm::Window::mouse.dx = 0;
  ulm::Window::mouse.dy = 0;

  ulm::Window::wheel.dx = 0;
  ulm::Window::wheel.dy = 0;

  for(int i = 1; i < MAX_CONTROLLERS; i++) ulm::Window::controllers[i].isPlugged = false;

  ulm::Window::controllers[0].isPlugged =     (bool)isPlugged;
  ulm::Window::controllers[0].up =            (bool)  up;
  ulm::Window::controllers[0].down =          (bool)  down;
  ulm::Window::controllers[0].left =          (bool)  left;
  ulm::Window::controllers[0].right =         (bool)  right;
  ulm::Window::controllers[0].start =         (bool)  start;
  ulm::Window::controllers[0].back =          (bool)  back;
  ulm::Window::controllers[0].leftShoulder =  (bool)  leftShoulder;
  ulm::Window::controllers[0].rightShoulder = (bool)  rightShoulder;
  ulm::Window::controllers[0].A =             (bool)  A;
  ulm::Window::controllers[0].B =             (bool)  B;
  ulm::Window::controllers[0].X =             (bool)  X;
  ulm::Window::controllers[0].Y =             (bool)  Y;
  ulm::Window::controllers[0].lX =            (float) lX;
  ulm::Window::controllers[0].lY =            (float) lY;
  ulm::Window::controllers[0].rX =            (float) rX;
  ulm::Window::controllers[0].rY =            (float) rY;
  ulm::Window::controllers[0].triggerLeft  =  (float) triggerLeft;
  ulm::Window::controllers[0].triggerRight =  (float) triggerRight;


  ulm::Window::updateKeys();
  currentProgram->update(deltaTime.count());

  if(BU_VR_initialized){
    if((curW != VR_fb_l.width || curH != VR_fb_l.height) || (curW != VR_fb_r.width || curH != VR_fb_r.height))
      VR_initFB();

    VR_fb_l.bind();
    VR_fb_l.clear(ulm::Window::clearColor);

    glBindFramebuffer(GL_FRAMEBUFFER, VR_fb_l.id);
    glViewport(0, 0, curW, curH);

    ulm::DefaultFrameBuffer::id = VR_fb_l.id;
    ulm::DefaultFrameBuffer::x  = 0;
    ulm::DefaultFrameBuffer::y  = 0;
    ulm::DefaultFrameBuffer::w  = curW;
    ulm::DefaultFrameBuffer::h  = curH;
    currentProgram->render(ulm::leftEye);


    VR_fb_r.bind();
    VR_fb_r.clear(ulm::Window::clearColor);
    glBindFramebuffer(GL_FRAMEBUFFER, VR_fb_r.id);
    glViewport(0, 0, curW, curH);

    ulm::DefaultFrameBuffer::id = VR_fb_r.id;
    ulm::DefaultFrameBuffer::x  = 0;
    ulm::DefaultFrameBuffer::y  = 0;
    ulm::DefaultFrameBuffer::w  = curW;
    ulm::DefaultFrameBuffer::h  = curH;

    currentProgram->render(ulm::rightEye);

  }

  glBindFramebuffer(GL_FRAMEBUFFER, form_id);
  glViewport(form_x, form_y, form_w, form_h);
}


JNIEXPORT jfloat JNICALL Java_org_Bibliotekum_1Ultimatum_app_BU_1Functions_nativeGetZNear(JNIEnv* env, jobject obj) { 
  return jfloat(ulm::CardboardVR::znear);
}

JNIEXPORT jfloat JNICALL Java_org_Bibliotekum_1Ultimatum_app_BU_1Functions_nativeGetZFar(JNIEnv* env, jobject obj) { 
  return jfloat(ulm::CardboardVR::zfar);
}


}  // extern "C"



#endif