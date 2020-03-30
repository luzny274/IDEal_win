#include "header.hpp"

class Main : public ulm::Program{
    public:
        float rotation = 0.0f;
        float time = 0.0f;
        float dt = 0.0f;
        long int numberOfFrames = 0;        

        ulm::Mesh cube;
        ulm::Mesh plane;
        ulm::Camera cam3D;
        ulm::GBuffer gbuffer;
        ulm::Sprite fullscreenSprite;

        ulm::SkyBox sky;

        float eyeDistance = 0.05;
        bool move = true;

        float d = 2.f;

        Main(){
            /* Initialization */

            ulm::Window::setVSync(false);
            ulm::Window::setRelativeMouseMode(move);

            /* Initializing skybox */
            sky.loadFromResources(
                "ft.jpg",
                "bk.jpg",
                "up.jpg",
                "dn.jpg",
                "rt.jpg",
                "lf.jpg");

            /* Initializing cube */
            cube.createTestCube();
            cube.setMaterial(ulm::Material(glm::vec4(0.8f, 0.3f, 0.8f, 1.f), 0.4, 0.3, 0.f));
            cube.initialize();

            cube.loadResourcesTextures("bricks.jpg", "bricks_material.jpg", "bricks_normal.png", "bricks_depth.jpg");

            cube.properties.parallax_minLayers = 8;
            cube.properties.parallax_maxLayers = 64;
            cube.properties.parallax_heightScale = 0.16f;
            cube.properties.parallax_repeatTexture = false;

            /* Initializing plane */
            plane.createTestCube();
            plane.setMaterial(ulm::Material(glm::vec4(0.4f, 0.3f, 0.4f, 1.f), 0.4, 0.3, 0.f));
            plane.initialize();
            plane.properties.modelMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, -5.f, 0.f));
            plane.properties.modelMatrix = glm::scale(plane.properties.modelMatrix, glm::vec3(50.f, 0.1f, 50.f));

            /* Initializing sprite for gbuffer result rendering */
            fullscreenSprite = ulm::SpriteFactory::createRectangle(-1.f, -1.f, 2.f, 2.f);

            /* Initialize 3D camera */
            cam3D.createPerspective(glm::radians(50.0f), (float)ulm::Window::screenSize.x / (float)ulm::Window::screenSize.y, 0.1f, 500.f);
            cam3D.position = glm::vec3(0.f, 0.f, -4.f);

            /* Initialize gbuffer */
            gbuffer.initialize(ulm::Window::screenSize);

            #ifdef BU_VR
                ulm::CardboardVR::setNearPlane(0.1f);
                ulm::CardboardVR::setFarPlane(500.f);
            #endif
        }

        float alpha = 1.5f;

        void update(float deltaTime){
            /* Run each frame */
            numberOfFrames++;
            time += deltaTime;
            dt = deltaTime;

            /* Rotate cube */
            cube.properties.modelMatrix = glm::rotate(glm::mat4(1.f), alpha, glm::vec3(-1.f, 1.f, -1.f));


            /* Handle input */
            
            if(ulm::Window::keysDown[ulm::SCANCODE_ESCAPE]){
                move = !move;
                ulm::Window::setRelativeMouseMode(move);
                if(move)
                    ulm::Window::setWindowMode(ulm::FULLSCREEN_DESKTOP);
                else
                    ulm::Window::setWindowMode(ulm::WINDOWED);
            }

            float viewSpeed = 3.f * deltaTime;

            if(ulm::Window::keysPressed[ulm::SCANCODE_LEFT])    cam3D.yaw(      viewSpeed);
            if(ulm::Window::keysPressed[ulm::SCANCODE_RIGHT])   cam3D.yaw(     -viewSpeed);
            if(ulm::Window::keysPressed[ulm::SCANCODE_UP])      cam3D.pitch(    viewSpeed);
            if(ulm::Window::keysPressed[ulm::SCANCODE_DOWN])    cam3D.pitch(   -viewSpeed);

            if(move){
                cam3D.yaw(  -(float)ulm::Window::mouse.dx  * 0.005f);
                cam3D.pitch(-(float)ulm::Window::mouse.dy  * 0.005f);
            }

            float speed = 4.8f * deltaTime;

            if(ulm::Window::keysPressed[ulm::SCANCODE_A])       cam3D.right(    -speed);
            if(ulm::Window::keysPressed[ulm::SCANCODE_D])       cam3D.right(     speed);
            if(ulm::Window::keysPressed[ulm::SCANCODE_W])       cam3D.forward(   speed);
            if(ulm::Window::keysPressed[ulm::SCANCODE_S])       cam3D.forward(  -speed);
            if(ulm::Window::keysPressed[ulm::SCANCODE_RSHIFT])  cam3D.higher(   -speed);
            if(ulm::Window::keysPressed[ulm::SCANCODE_SPACE])   cam3D.higher(    speed);//*/

            if(ulm::Window::controllerExists()){
                if(abs(ulm::Window::getFirstPluggedController().lY) > 0.1f) cam3D.forward(ulm::Window::getFirstPluggedController().lY * speed);
                if(abs(ulm::Window::getFirstPluggedController().lX) > 0.1f) cam3D.right(ulm::Window::getFirstPluggedController().lX * speed);

                cam3D.yaw(-ulm::Window::getFirstPluggedController().rX * viewSpeed);
                cam3D.pitch(-ulm::Window::getFirstPluggedController().rY * viewSpeed);
            }
        }

        int color      = false;
        int multiply   = true;
        int diffuse    = true;
        int albedo     = true;
        int normal     = true;
        int parallax   = false;
        int blend      = true;
        int aether     = false;

        void render(ulm::Eye eye){
            /* Render frame */
            /* Create flags for cube rendering */

            std::uint_fast16_t flags = 0;
            if(color   )       flags |= ulm::Colored       ;
            if(multiply)       flags |= ulm::Multiplied    ;
            if(diffuse )       flags |= ulm::DiffuseMapped ;
            if(albedo  )       flags |= ulm::AlbedoMapped  ;
            if(normal  )       flags |= ulm::NormalMapped  ;
            if(parallax)       flags |= ulm::ParallaxMapped;
            if(blend   )       flags |= ulm::Blended       ;
            if(aether  )       flags |= ulm::Aether        ;

            /* 3D */
            #ifdef BU_VR
                /* Handle VR */
                cam3D.projection =    ulm::CardboardVR::getPerspective();
                cam3D.up =            ulm::CardboardVR::getUp();
                cam3D.front =         ulm::CardboardVR::getFront();
                cam3D.position +=     (float)(eye) * ulm::CardboardVR::getRight() * eyeDistance;
            #endif

            ulm::Window::getGLError(true);
            gbuffer.clear();
            
            /* Update geometry buffer */
            ulm::Renderer3D::begin(cam3D, &gbuffer);
            ulm::Renderer3D::submit(cube, flags);
            ulm::Renderer3D::submit(plane, ulm::Colored);
            ulm::Renderer3D::draw(ulm::CullBackFace);

            /* Handle lights */
            ulm::LightManager::begin(cam3D, &gbuffer);
            ulm::LightManager::submit(ulm::DirectionalLight(glm::vec3(-1.f, -1.f, -1.f), glm::vec3(1.f), 0.2f));
            ulm::LightManager::draw();

            ulm::SkyBoxRenderer::drawSkyBox(sky, &gbuffer, cam3D);
            ulm::Renderer3D::drawAether(ulm::CullNone);

            fullscreenSprite.texture = gbuffer.getResult();

            /* Draw the result from gbuffer to screen */
            ulm::Renderer2D::begin(glm::mat4(1.f));
            ulm::Renderer2D::draw(fullscreenSprite, ulm::Textured | ulm::Multiplied);
            ulm::Renderer2D::end();


            /* GUI */

            ulm::NK::processInput(ulm::Window::mouse, ulm::Window::wheel,
                                  ulm::Window::keysPressed, ulm::Window::keysDown, ulm::Window::keysUp, 
                                  ulm::Window::textInput);

            if (nk_begin(ulm::NK::ctx, "Demo", nk_rect(20, 20, 300, 400),
                NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
                NK_WINDOW_CLOSABLE|NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
            {
                nk_layout_row_dynamic(ulm::NK::ctx, 20, 1); 
                nk_label_wrap(ulm::NK::ctx, (ulm::String("current dt: ") + dt + " ms").getPtr());

                nk_layout_row_dynamic(ulm::NK::ctx, 20, 1); 
                nk_label_wrap(ulm::NK::ctx, (ulm::String("average dt: ") + (time / (float)numberOfFrames) + " ms").getPtr());
                nk_layout_row_dynamic(ulm::NK::ctx, 20, 1); 
                nk_label_wrap(ulm::NK::ctx, (ulm::String("average framerate: ") + ((float)numberOfFrames / time) + " fps").getPtr());

                nk_layout_row_dynamic(ulm::NK::ctx, 30, 2); 
                nk_label_wrap(ulm::NK::ctx, "Alpha:"); nk_slider_float(ulm::NK::ctx, 0.0f, &cube.properties.diffuseMultiplier.w, 1.f, 0.01f);

                nk_layout_row_dynamic(ulm::NK::ctx, 20, 1); nk_checkbox_label(ulm::NK::ctx, "Colored", &color   );
                nk_layout_row_dynamic(ulm::NK::ctx, 20, 1); nk_checkbox_label(ulm::NK::ctx, "Multiplied", &multiply);
                nk_layout_row_dynamic(ulm::NK::ctx, 20, 1); nk_checkbox_label(ulm::NK::ctx, "DiffuseMapped", &diffuse );
                nk_layout_row_dynamic(ulm::NK::ctx, 20, 1); nk_checkbox_label(ulm::NK::ctx, "AlbedoMapped", &albedo  );
                nk_layout_row_dynamic(ulm::NK::ctx, 20, 1); nk_checkbox_label(ulm::NK::ctx, "NormalMapped", &normal  );
                nk_layout_row_dynamic(ulm::NK::ctx, 20, 1); nk_checkbox_label(ulm::NK::ctx, "ParallaxMapped", &parallax);
                nk_layout_row_dynamic(ulm::NK::ctx, 20, 1); nk_checkbox_label(ulm::NK::ctx, "Blended", &blend   );
                nk_layout_row_dynamic(ulm::NK::ctx, 20, 1); nk_checkbox_label(ulm::NK::ctx, "Aether", &aether  );
            }

            nk_end(ulm::NK::ctx);
            ulm::NK::draw();//*/

            #ifdef BU_VR
                /* Handle VR */
                cam3D.position -= (float)eye * ulm::CardboardVR::getRight() * eyeDistance;
            #endif 
        }

        void resizeCallback(int width, int height){
            /* When resize */
        }

        ~Main(){
            /* Destruction */
        }
};

ulm::Program * ulm::Properties::onStart(){
    /* Window initialization */
    ulm::Window::initialize(BU_APP_NAME, 256, 256);
    ulm::Window::maximize(true);
    ulm::Window::setWindowMode(ulm::FULLSCREEN_DESKTOP);
    return new Main();
}

void ulm::Properties::handleError(ulm::String error){
    /* Print errors */
    printf("\n%s\n", error.getPtr());
    fflush(stdout);
}