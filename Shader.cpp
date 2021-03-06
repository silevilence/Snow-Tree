//
// Created by lenovo on 2017/9/13.
//

#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

Shader &Shader::use() {
    glUseProgram(this->id);
    return *this;
}

void Shader::compile(const GLchar *vertexSource, const GLchar *fragmentSource, const GLchar *geometrySource) {
    GLuint sVertex, sFragment, gShader = 0;
    // Vertex Shader
    sVertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sVertex, 1, &vertexSource, nullptr);
    glCompileShader(sVertex);
    check_compile_errors(sVertex, "VERTEX");
    // Fragment Shader
    sFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(sFragment, 1, &fragmentSource, nullptr);
    glCompileShader(sFragment);
    check_compile_errors(sFragment, "FRAGMENT");
    // If geometry shader source code is given, also compile geometry shader
    if(geometrySource != nullptr) {
        gShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(gShader, 1, &geometrySource, nullptr);
        glCompileShader(gShader);
        check_compile_errors(gShader, "GEOMETRY");
    }
    // Shader Program
    this->id = glCreateProgram();
    glAttachShader(this->id, sVertex);
    glAttachShader(this->id, sFragment);
    if(geometrySource != nullptr)
        glAttachShader(this->id, gShader);
    glLinkProgram(this->id);
    check_compile_errors(this->id, "PROGRAM");
    // Delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(sVertex);
    glDeleteShader(sFragment);
    if(geometrySource != nullptr)
        glDeleteShader(gShader);
}

void Shader::set_float(const GLchar *name, GLfloat value, GLboolean useShader) {
    if(useShader)
        this->use();
    glUniform1f(glGetUniformLocation(this->id, name), value);
}

void Shader::set_integer(const GLchar *name, GLint value, GLboolean useShader) {
    if(useShader)
        this->use();
    glUniform1i(glGetUniformLocation(this->id, name), value);
}

void Shader::set_vector2f(const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader) {
    if(useShader)
        this->use();
    glUniform2f(glGetUniformLocation(this->id, name), x, y);
}

void Shader::set_vector2f(const GLchar *name, const glm::vec2 &value, GLboolean useShader) {
    if(useShader)
        this->use();
    glUniform2f(glGetUniformLocation(this->id, name), value.x, value.y);
}

void Shader::set_vector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader) {
    if(useShader)
        this->use();
    glUniform3f(glGetUniformLocation(this->id, name), x, y, z);
}

void Shader::set_vector3f(const GLchar *name, const glm::vec3 &value, GLboolean useShader) {
    if(useShader)
        this->use();
    glUniform3f(glGetUniformLocation(this->id, name), value.x, value.y, value.z);
}

void Shader::set_vector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader) {
    if(useShader)
        this->use();
    glUniform4f(glGetUniformLocation(this->id, name), x, y, z, w);
}

void Shader::set_vector4f(const GLchar *name, const glm::vec4 &value, GLboolean useShader) {
    if(useShader)
        this->use();
    glUniform4f(glGetUniformLocation(this->id, name), value.x, value.y, value.z, value.w);
}

void Shader::set_matrix4(const GLchar *name, const glm::mat4 &matrix, GLboolean useShader) {
    if(useShader)
        this->use();
    glUniformMatrix4fv(glGetUniformLocation(this->id, name), 1, GL_FALSE, glm::value_ptr(matrix));
}


void Shader::check_compile_errors(GLuint object, const std::string &type) {
    GLint success;
    GLchar infoLog[1024];
    if(type != "PROGRAM") {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(object, 1024, nullptr, infoLog);
            std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- "
                      << std::endl;
        }
    } else {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if(!success) {
            glGetProgramInfoLog(object, 1024, nullptr, infoLog);
            std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- "
                      << std::endl;
        }
    }
}

void Shader::compile_from_file(const GLchar *vertex_path, const GLchar *fragment_path, const GLchar *geometry_path) {
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // open files
        vShaderFile.open(vertex_path);
        fShaderFile.open(fragment_path);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        // if geometry shader path is present, also load a geometry shader
        if(geometry_path != nullptr) {
            gShaderFile.open(geometry_path);
            std::stringstream gShaderStream;
            gShaderStream << gShaderFile.rdbuf();
            gShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    } catch(std::ifstream::failure e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();
    const char *gShaderCode = nullptr;
    if(geometry_path != nullptr) {
        gShaderCode = geometryCode.c_str();
    }

    this->compile(vShaderCode, fShaderCode, gShaderCode);
}
