#ifndef functionsGL_BU
#define functionsGL_BU

#include "ToolsGL.hpp"


GLenum ulm::Types::toGL(Type typ){
    switch(typ){
        case ulm::NO_TYPE:
            return 0;
            break;

        case ulm::FLOAT:
            return GL_FLOAT;
            break;

        case ulm::INT:
            return GL_INT;
            break;

        case ulm::UINT:
            return GL_UNSIGNED_INT;
            break;

        case ulm::CHAR:
            return GL_BYTE;
            break;

        case ulm::BYTE:
            return GL_BYTE;
            break;
    }

    return 0;
}

ulm::Type ulm::Types::fromGL(GLenum typ){
    switch(typ){
        case GL_FLOAT:
            return ulm::FLOAT;
            break;

        case GL_INT:
            return ulm::INT;
            break;

        case GL_UNSIGNED_INT:
            return ulm::UINT;
            break;

        case GL_BYTE:
            return ulm::BYTE;
            break;
    }

    return ulm::NO_TYPE;
}


static unsigned int compileShader(unsigned int type, const char * source);
unsigned int createShaderProgram(const char * vertexShader, const char * fragmentShader);
#ifndef BU_MOBILE
    unsigned int createShaderProgram(const char * vertexShader, const char * geometryShader, const char * fragmentShader);
#endif

static unsigned int compileShader(unsigned int type, const char * source){
    unsigned int id = glCreateShader(type);

    glShaderSource(id, 1, &source, NULL);
    glCompileShader(id);

    int  success;
    char infoLog[512];
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);

    if(!success){
        glGetShaderInfoLog(id, 512, NULL, infoLog);

        ulm::String error("ULM::ERROR::compileShader::");
        if(type == GL_VERTEX_SHADER) error += "VERTEX SHADER";
        else if(type == GL_FRAGMENT_SHADER) error += "FRAGMENT SHADER";
        #ifndef BU_MOBILE
            else if(type == GL_GEOMETRY_SHADER) error += "GEOMETRY SHADER";
        #endif
        ulm::Properties::handleError(error + "::COMPILATION_FAILED\n" + infoLog);
    }

    return(id);
}

#ifndef BU_MOBILE
    unsigned int createShaderProgram(const char * vertexShader, const char * geometryShader, const char * fragmentShader){
        unsigned int program = glCreateProgram();

        unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
        unsigned int gs = compileShader(GL_GEOMETRY_SHADER, geometryShader);
        unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

        glAttachShader(program, vs);
        glAttachShader(program, gs);
        glAttachShader(program, fs);

        glLinkProgram(program);
        glValidateProgram(program);

        int success;
        char infoLog[512];
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if(!success) {
            glGetProgramInfoLog(program, 512, NULL, infoLog);
            ulm::Properties::handleError(ulm::String("ULM::ERROR::createShaderProgram::LINKING_FAILED\n") + infoLog);
        }

        glDeleteShader(vs);
        glDeleteShader(gs);
        glDeleteShader(fs);  

        return(program);
    }
#endif

unsigned int createShaderProgram(const char * vertexShader, const char * fragmentShader){
    unsigned int program = glCreateProgram();

    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        ulm::Properties::handleError(ulm::String("ULM::ERROR::createShaderProgram::LINKING_FAILED\n") + infoLog);
    }

    glDeleteShader(vs);
    glDeleteShader(fs);  

    return(program);
}

#endif