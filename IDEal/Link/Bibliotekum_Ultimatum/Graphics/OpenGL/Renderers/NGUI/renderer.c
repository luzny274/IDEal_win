#ifndef NK_SDL_GLES2_H_BU_REND
#define NK_SDL_GLES2_H_BU_REND

#include "../../../Core/Core.hpp"
#include "../../Tools/ToolsGL.hpp"



NK_API struct nk_context*   nk_sdl_init();
NK_API void                 nk_sdl_font_stash_begin(struct nk_font_atlas **atlas);
NK_API void                 nk_sdl_font_stash_end(void);
NK_API void                 nk_sdl_render(enum nk_anti_aliasing , int max_vertex_buffer, int max_element_buffer);
NK_API void                 nk_sdl_shutdown(void);
NK_API void                 nk_sdl_device_destroy(void);
NK_API void                 nk_sdl_device_create(void);

struct nk_sdl_device {
    struct nk_buffer cmds;
    struct nk_draw_null_texture null;
    GLuint vbo, ebo;
    GLuint prog;
    GLuint vert_shdr;
    GLuint frag_shdr;
    GLint attrib_pos;
    GLint attrib_uv;
    GLint attrib_col;
    GLint uniform_tex;
    GLint uniform_proj;
    ulm::Texture font_tex;
    GLsizei vs;
    size_t vp, vt, vc;
};

struct nk_sdl_vertex {
    GLfloat position[2];
    GLfloat uv[2];
    nk_byte col[4];
};

static struct nk_sdl {
    struct nk_sdl_device ogl;
    struct nk_context ctx;
    struct nk_font_atlas atlas;
} sdl;


#define NK_SHADER_VERSION "#version 100\n"


NK_API void
nk_sdl_device_create(void)
{
    GLint status;
    static const GLchar *vertex_shader =
        NK_SHADER_VERSION
        "uniform mat4 ProjMtx;\n"
        "attribute vec2 Position;\n"
        "attribute vec2 TexCoord;\n"
        "attribute vec4 Color;\n"
        "varying vec2 Frag_UV;\n"
        "varying vec4 Frag_Color;\n"
        "void main() {\n"
        "   Frag_UV = TexCoord;\n"
        "   Frag_Color = Color;\n"
        "   gl_Position = ProjMtx * vec4(Position.xy, 0, 1);\n"
        "}\n";
    static const GLchar *fragment_shader =
        NK_SHADER_VERSION
        "precision mediump float;\n"
        "uniform sampler2D Texture;\n"
        "varying vec2 Frag_UV;\n"
        "varying vec4 Frag_Color;\n"
        "void main(){\n"
        "   gl_FragColor = Frag_Color * texture2D(Texture, Frag_UV);\n"
        "}\n";

    struct nk_sdl_device *dev = &sdl.ogl;
    
    nk_buffer_init_default(&dev->cmds);
    dev->prog = glCreateProgram();
    dev->vert_shdr = glCreateShader(GL_VERTEX_SHADER);
    dev->frag_shdr = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(dev->vert_shdr, 1, &vertex_shader, 0);
    glShaderSource(dev->frag_shdr, 1, &fragment_shader, 0);
    glCompileShader(dev->vert_shdr);
    glCompileShader(dev->frag_shdr);
    glGetShaderiv(dev->vert_shdr, GL_COMPILE_STATUS, &status);
    assert(status == GL_TRUE);
    glGetShaderiv(dev->frag_shdr, GL_COMPILE_STATUS, &status);
    assert(status == GL_TRUE);
    glAttachShader(dev->prog, dev->vert_shdr);
    glAttachShader(dev->prog, dev->frag_shdr);
    glLinkProgram(dev->prog);
    glGetProgramiv(dev->prog, GL_LINK_STATUS, &status);
    assert(status == GL_TRUE);


    dev->uniform_tex = glGetUniformLocation(dev->prog, "Texture");
    dev->uniform_proj = glGetUniformLocation(dev->prog, "ProjMtx");
    dev->attrib_pos = glGetAttribLocation(dev->prog, "Position");
    dev->attrib_uv = glGetAttribLocation(dev->prog, "TexCoord");
    dev->attrib_col = glGetAttribLocation(dev->prog, "Color");
    {
        dev->vs = sizeof(struct nk_sdl_vertex);
        dev->vp = offsetof(struct nk_sdl_vertex, position);
        dev->vt = offsetof(struct nk_sdl_vertex, uv);
        dev->vc = offsetof(struct nk_sdl_vertex, col);
        
        /* Allocate buffers */
        glGenBuffers(1, &dev->vbo);
        glGenBuffers(1, &dev->ebo);
    }
    //glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

NK_INTERN void
nk_sdl_device_upload_atlas(const void *image, int width, int height)
{
    ulm::Image font_image(4, width, height, (unsigned char *)image);
    struct nk_sdl_device *dev = &sdl.ogl;
    //dev->font_tex.initialize(font_image, false, ulm::NEAREST, true);

    dev->font_tex.initialize(font_image, false, ulm::TEXTURE_ROUND_UP , ulm::NEAREST, true);

    /*
    struct nk_sdl_device *dev = &sdl.ogl;
    glGenTextures(1, &dev->font_tex);
    glBindTexture(GL_TEXTURE_2D, dev->font_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, image);
                //*/
}

NK_API void
nk_sdl_device_destroy(void)
{
    struct nk_sdl_device *dev = &sdl.ogl;
    glDetachShader(dev->prog, dev->vert_shdr);
    glDetachShader(dev->prog, dev->frag_shdr);
    glDeleteShader(dev->vert_shdr);
    glDeleteShader(dev->frag_shdr);
    glDeleteProgram(dev->prog);
    //glDeleteTextures(1, &dev->font_tex);
    dev->font_tex.end();
    glDeleteBuffers(1, &dev->vbo);
    glDeleteBuffers(1, &dev->ebo);
    nk_buffer_free(&dev->cmds);
}

NK_API void
nk_sdl_render(enum nk_anti_aliasing AA, int max_vertex_buffer, int max_element_buffer, int width, int height)
{
    struct nk_sdl_device *dev = &sdl.ogl;
    //int width, height;
    //int display_width, display_height;
    struct nk_vec2 scale;

    
    GLfloat ortho[4][4] = {
        {2.0f, 0.0f, 0.0f, 0.0f},
        {0.0f,-2.0f, 0.0f, 0.0f},
        {0.0f, 0.0f,-1.0f, 0.0f},
        {-1.0f,1.0f, 0.0f, 1.0f},
    };
    //*/
/*
    width = ulm::Window.width;
    height = ulm::Window.height;//*/
    int display_width = ulm::Window::width;
    int display_height = ulm::Window::height;

    ortho[0][0] /= (GLfloat)width;
    ortho[1][1] /= (GLfloat)height;

    scale.x = (float)display_width/(float)width;
    scale.y = (float)display_height/(float)height;

    /* setup global state */
    //glViewport(0,0,display_width,display_height);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    //glActiveTexture(GL_TEXTURE0);

    /* setup program */
    glUseProgram(dev->prog);
    glUniformMatrix4fv(dev->uniform_proj, 1, GL_FALSE, &ortho[0][0]);
    {
        /* convert from command queue into draw list and draw to screen */
        const struct nk_draw_command *cmd;
        void *vertices, *elements;
        const nk_draw_index *offset = NULL;

        /* Bind buffers */
        glBindBuffer(GL_ARRAY_BUFFER, dev->vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dev->ebo);
        
        {
            /* buffer setup */
            glEnableVertexAttribArray((GLuint)dev->attrib_pos);
            glEnableVertexAttribArray((GLuint)dev->attrib_uv);
            glEnableVertexAttribArray((GLuint)dev->attrib_col);

            glVertexAttribPointer((GLuint)dev->attrib_pos, 2, GL_FLOAT, GL_FALSE, dev->vs, (void*)dev->vp);
            glVertexAttribPointer((GLuint)dev->attrib_uv, 2, GL_FLOAT, GL_FALSE, dev->vs, (void*)dev->vt);
            glVertexAttribPointer((GLuint)dev->attrib_col, 4, GL_UNSIGNED_BYTE, GL_TRUE, dev->vs, (void*)dev->vc);
        }

        glBufferData(GL_ARRAY_BUFFER, max_vertex_buffer, NULL, GL_STREAM_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, max_element_buffer, NULL, GL_STREAM_DRAW);

        /* load vertices/elements directly into vertex/element buffer */
        vertices = malloc((size_t)max_vertex_buffer);
        elements = malloc((size_t)max_element_buffer);
        {
            /* fill convert configuration */
            struct nk_convert_config config;
            static const struct nk_draw_vertex_layout_element vertex_layout[] = {
                {NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_sdl_vertex, position)},
                {NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_sdl_vertex, uv)},
                {NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, NK_OFFSETOF(struct nk_sdl_vertex, col)},
                {NK_VERTEX_LAYOUT_END}
            };
            NK_MEMSET(&config, 0, sizeof(config));
            config.vertex_layout = vertex_layout;
            config.vertex_size = sizeof(struct nk_sdl_vertex);
            config.vertex_alignment = NK_ALIGNOF(struct nk_sdl_vertex);
            config.null = dev->null;
            config.circle_segment_count = 22;
            config.curve_segment_count = 22;
            config.arc_segment_count = 22;
            config.global_alpha = 1.0f;
            config.shape_AA = AA;
            config.line_AA = AA;

            /* setup buffers to load vertices and elements */
            {struct nk_buffer vbuf, ebuf;
            nk_buffer_init_fixed(&vbuf, vertices, (nk_size)max_vertex_buffer);
            nk_buffer_init_fixed(&ebuf, elements, (nk_size)max_element_buffer);
            nk_convert(&sdl.ctx, &dev->cmds, &vbuf, &ebuf, &config);}
        }
        glBufferSubData(GL_ARRAY_BUFFER, 0, (size_t)max_vertex_buffer, vertices);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, (size_t)max_element_buffer, elements);
        free(vertices);
        free(elements);

        /* iterate over and execute each draw command */
        nk_draw_foreach(cmd, &sdl.ctx, &dev->cmds) {
            if (!cmd->elem_count) continue;
            //glBindTexture(GL_TEXTURE_2D, (GLuint)cmd->texture.id);

            int slot = ulm::TextureManager::use((int)cmd->texture.id);
            glUniform1i(dev->uniform_tex, slot);

            glScissor((GLint)(cmd->clip_rect.x * scale.x + (float)ulm::DefaultFrameBuffer::x),
                (GLint)((height - (GLint)(cmd->clip_rect.y + cmd->clip_rect.h)) * scale.y - (float)ulm::DefaultFrameBuffer::y),
                (GLint)(cmd->clip_rect.w * scale.x),
                (GLint)(cmd->clip_rect.h * scale.y));
            glDrawElements(GL_TRIANGLES, (GLsizei)cmd->elem_count, GL_UNSIGNED_SHORT, offset);
            offset += cmd->elem_count;
        }
        nk_clear(&sdl.ctx);
    }

    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
}

static void
nk_sdl_clipboard_paste(nk_handle usr, struct nk_text_edit *edit)
{
    const char *text = ulm::Window::getClipboardText();
    if (text) nk_textedit_paste(edit, text, nk_strlen(text));
    (void)usr;
}

static void
nk_sdl_clipboard_copy(nk_handle usr, const char *text, int len)
{
    char *str = 0;
    (void)usr;
    if (!len) return;
    str = (char*)malloc((size_t)len+1);
    if (!str) return;
    memcpy(str, text, (size_t)len);
    str[len] = '\0';
    ulm::Window::setClipboardText(str);
    free(str);
}

NK_API struct nk_context*
nk_sdl_init()
{
    nk_init_default(&sdl.ctx, 0);
    sdl.ctx.clip.copy = nk_sdl_clipboard_copy;
    sdl.ctx.clip.paste = nk_sdl_clipboard_paste;
    sdl.ctx.clip.userdata = nk_handle_ptr(0);
    nk_sdl_device_create();
    return &sdl.ctx;
}

NK_API void
nk_sdl_font_stash_begin(struct nk_font_atlas **atlas)
{
    nk_font_atlas_init_default(&sdl.atlas);
    nk_font_atlas_begin(&sdl.atlas);
    *atlas = &sdl.atlas;
}

NK_API void
nk_sdl_font_stash_end(void)
{
    const void *image; int w, h;
    image = nk_font_atlas_bake(&sdl.atlas, &w, &h, NK_FONT_ATLAS_RGBA32);

    nk_sdl_device_upload_atlas(image, w, h);


    nk_font_atlas_end(&sdl.atlas, nk_handle_id((int)sdl.ogl.font_tex.id), &sdl.ogl.null);
    if (sdl.atlas.default_font)
        nk_style_set_font(&sdl.ctx, &sdl.atlas.default_font->handle);
}

NK_API void
nk_sdl_font_stash_end(int fontHeight)
{
    const void *image; int w, h;
    image = nk_font_atlas_bake(&sdl.atlas, &w, &h, NK_FONT_ATLAS_RGBA32);

    nk_sdl_device_upload_atlas(image, w, h);


    nk_font_atlas_end(&sdl.atlas, nk_handle_id((int)sdl.ogl.font_tex.id), &sdl.ogl.null);
    sdl.atlas.default_font->handle.height = (float)fontHeight;
    if (sdl.atlas.default_font)
        nk_style_set_font(&sdl.ctx, &sdl.atlas.default_font->handle);
}

/*
NK_API int
nk_sdl_handle_event(SDL_Event *evt)
{
    struct nk_context *ctx = &sdl.ctx;
    if (evt->type == SDL_KEYUP || evt->type == SDL_KEYDOWN) {
        
        int down = evt->type == SDL_KEYDOWN;
        const Uint8* state = SDL_GetKeyboardState(0);
        SDL_Keycode sym = evt->key.keysym.sym;
        if (sym == SDLK_RSHIFT || sym == SDLK_LSHIFT)
            nk_input_key(ctx, NK_KEY_SHIFT, down);
        else if (sym == SDLK_DELETE)
            nk_input_key(ctx, NK_KEY_DEL, down);
        else if (sym == SDLK_RETURN)
            nk_input_key(ctx, NK_KEY_ENTER, down);
        else if (sym == SDLK_TAB)
            nk_input_key(ctx, NK_KEY_TAB, down);
        else if (sym == SDLK_BACKSPACE)
            nk_input_key(ctx, NK_KEY_BACKSPACE, down);
        else if (sym == SDLK_HOME) {
            nk_input_key(ctx, NK_KEY_TEXT_START, down);
            nk_input_key(ctx, NK_KEY_SCROLL_START, down);
        } else if (sym == SDLK_END) {
            nk_input_key(ctx, NK_KEY_TEXT_END, down);
            nk_input_key(ctx, NK_KEY_SCROLL_END, down);
        } else if (sym == SDLK_PAGEDOWN) {
            nk_input_key(ctx, NK_KEY_SCROLL_DOWN, down);
        } else if (sym == SDLK_PAGEUP) {
            nk_input_key(ctx, NK_KEY_SCROLL_UP, down);
        } else if (sym == SDLK_z)
            nk_input_key(ctx, NK_KEY_TEXT_UNDO, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_r)
            nk_input_key(ctx, NK_KEY_TEXT_REDO, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_c)
            nk_input_key(ctx, NK_KEY_COPY, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_v)
            nk_input_key(ctx, NK_KEY_PASTE, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_x)
            nk_input_key(ctx, NK_KEY_CUT, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_b)
            nk_input_key(ctx, NK_KEY_TEXT_LINE_START, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_e)
            nk_input_key(ctx, NK_KEY_TEXT_LINE_END, down && state[SDL_SCANCODE_LCTRL]);
        else if (sym == SDLK_UP)
            nk_input_key(ctx, NK_KEY_UP, down);
        else if (sym == SDLK_DOWN)
            nk_input_key(ctx, NK_KEY_DOWN, down);
        else if (sym == SDLK_LEFT) {
            if (state[SDL_SCANCODE_LCTRL])
                nk_input_key(ctx, NK_KEY_TEXT_WORD_LEFT, down);
            else nk_input_key(ctx, NK_KEY_LEFT, down);
        } else if (sym == SDLK_RIGHT) {
            if (state[SDL_SCANCODE_LCTRL])
                nk_input_key(ctx, NK_KEY_TEXT_WORD_RIGHT, down);
            else nk_input_key(ctx, NK_KEY_RIGHT, down);
        } else return 0;
        return 1;
    } else if (evt->type == SDL_MOUSEBUTTONDOWN || evt->type == SDL_MOUSEBUTTONUP) {
        
        int down = evt->type == SDL_MOUSEBUTTONDOWN;
        const int x = evt->button.x, y = evt->button.y;
        if (evt->button.button == SDL_BUTTON_LEFT) {
            if (evt->button.clicks > 1)
                nk_input_button(ctx, NK_BUTTON_DOUBLE, x, y, down);
            nk_input_button(ctx, NK_BUTTON_LEFT, x, y, down);
        } else if (evt->button.button == SDL_BUTTON_MIDDLE)
            nk_input_button(ctx, NK_BUTTON_MIDDLE, x, y, down);
        else if (evt->button.button == SDL_BUTTON_RIGHT)
            nk_input_button(ctx, NK_BUTTON_RIGHT, x, y, down);
        return 1;
    } else if (evt->type == SDL_MOUSEMOTION) {
        if (ctx->input.mouse.grabbed) {
            int x = (int)ctx->input.mouse.prev.x, y = (int)ctx->input.mouse.prev.y;
            nk_input_motion(ctx, x + evt->motion.xrel, y + evt->motion.yrel);
        } else nk_input_motion(ctx, evt->motion.x, evt->motion.y);
        return 1;
    } else if (evt->type == SDL_TEXTINPUT) {
        nk_glyph glyph;
        memcpy(glyph, evt->text.text, NK_UTF_SIZE);
        nk_input_glyph(ctx, glyph);
        return 1;
    } else if (evt->type == SDL_MOUSEWHEEL) {
        nk_input_scroll(ctx,nk_vec2((float)evt->wheel.x,(float)evt->wheel.y));
        return 1;
    }
    return 0;
}//*/


NK_API int
nk_handle_input(ulm::MOUSE mouse,
                ulm::WHEEL wheel,
                char * keysPressed, char * keysDown, char * keysUp,
                char * textInput, int w, int h)
{
    struct nk_context *ctx = &sdl.ctx;

    /* keys */
    {
        nk_input_key(ctx, NK_KEY_SHIFT,                 keysPressed[ulm::SCANCODE_RSHIFT] || keysPressed[ulm::SCANCODE_LSHIFT]);
        nk_input_key(ctx, NK_KEY_DEL,                   keysPressed[ulm::SCANCODE_DELETE] );
        nk_input_key(ctx, NK_KEY_ENTER,                 keysDown   [ulm::SCANCODE_KP_ENTER] || keysDown[ulm::SCANCODE_RETURN]);
        nk_input_key(ctx, NK_KEY_TAB,                   keysPressed[ulm::SCANCODE_TAB] );
        nk_input_key(ctx, NK_KEY_BACKSPACE,             keysDown   [ulm::SCANCODE_BACKSPACE] );
        nk_input_key(ctx, NK_KEY_TEXT_START,            keysPressed[ulm::SCANCODE_HOME] );
        nk_input_key(ctx, NK_KEY_SCROLL_START,          keysPressed[ulm::SCANCODE_HOME] );
        nk_input_key(ctx, NK_KEY_TEXT_END,              keysPressed[ulm::SCANCODE_END] );
        nk_input_key(ctx, NK_KEY_SCROLL_END,            keysPressed[ulm::SCANCODE_END] );
        nk_input_key(ctx, NK_KEY_SCROLL_DOWN,           keysPressed[ulm::SCANCODE_PAGEDOWN] );
        nk_input_key(ctx, NK_KEY_SCROLL_UP,             keysPressed[ulm::SCANCODE_PAGEUP] );
        nk_input_key(ctx, NK_KEY_TEXT_UNDO,             keysPressed[ulm::SCANCODE_Z] && keysPressed[ulm::SCANCODE_LCTRL]);
        nk_input_key(ctx, NK_KEY_TEXT_REDO,             keysPressed[ulm::SCANCODE_R] && keysPressed[ulm::SCANCODE_LCTRL]);
        nk_input_key(ctx, NK_KEY_COPY,                  keysPressed[ulm::SCANCODE_C] && keysPressed[ulm::SCANCODE_LCTRL]);
        nk_input_key(ctx, NK_KEY_PASTE,                 keysPressed[ulm::SCANCODE_V] && keysPressed[ulm::SCANCODE_LCTRL]);
        nk_input_key(ctx, NK_KEY_CUT,                   keysPressed[ulm::SCANCODE_X] && keysPressed[ulm::SCANCODE_LCTRL]);
        nk_input_key(ctx, NK_KEY_TEXT_LINE_START,       keysPressed[ulm::SCANCODE_B] && keysPressed[ulm::SCANCODE_LCTRL]);
        nk_input_key(ctx, NK_KEY_TEXT_LINE_END,         keysPressed[ulm::SCANCODE_E] && keysPressed[ulm::SCANCODE_LCTRL]);
        nk_input_key(ctx, NK_KEY_UP,                    keysPressed[ulm::SCANCODE_UP]);
        nk_input_key(ctx, NK_KEY_DOWN,                  keysPressed[ulm::SCANCODE_DOWN]);

    if (ulm::Window::keysPressed[ulm::SCANCODE_LCTRL])    
        nk_input_key(ctx, NK_KEY_TEXT_WORD_LEFT,        keysPressed[ulm::SCANCODE_LEFT]);
    else nk_input_key(ctx, NK_KEY_LEFT,                 keysPressed[ulm::SCANCODE_LEFT]);

    if (ulm::Window::keysPressed[ulm::SCANCODE_LCTRL])   
        nk_input_key(ctx, NK_KEY_TEXT_WORD_RIGHT,       keysPressed[ulm::SCANCODE_RIGHT]);
    else nk_input_key(ctx, NK_KEY_RIGHT,                keysPressed[ulm::SCANCODE_RIGHT]);
    }   


    /* mouse */ 
    if(!mouse.relativeMode)
    {   
        glm::ivec2 tmpmouse = glm::ivec2(mouse.x * w / ulm::Window::width, mouse.y * h / ulm::Window::height);

        nk_input_button(ctx, NK_BUTTON_LEFT,      (int) tmpmouse.x, (int) tmpmouse.y, keysPressed[ulm::SCANCODE_LCLICK]);
        nk_input_button(ctx, NK_BUTTON_MIDDLE,    (int) tmpmouse.x, (int) tmpmouse.y, keysPressed[ulm::SCANCODE_MCLICK]);
        nk_input_button(ctx, NK_BUTTON_RIGHT,     (int) tmpmouse.x, (int) tmpmouse.y, keysPressed[ulm::SCANCODE_RCLICK]);
        nk_input_motion(ctx,                      (int) tmpmouse.x, (int) tmpmouse.y);

        nk_input_scroll(ctx,nk_vec2(            (float) wheel.dx, (float) wheel.dy));
    }

    /* text input */
    if(textInput[0] != '\0' && textInput[0] != '\n'){
        nk_glyph glyph;
        memcpy(glyph,    textInput, NK_UTF_SIZE);
        nk_input_glyph(ctx, glyph);
    }


    return 0;
}

NK_API
void nk_sdl_shutdown(void)
{
    nk_font_atlas_clear(&sdl.atlas);
    nk_free(&sdl.ctx);
    nk_sdl_device_destroy();
    memset((void*)&sdl, 0, sizeof(sdl));
}

#endif
