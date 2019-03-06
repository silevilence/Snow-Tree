//
// Created by lenovo on 2017/9/13.
//

#pragma once
#ifndef LEARNOPENGL_SHADER_H
#define LEARNOPENGL_SHADER_H

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
	// State
	GLuint id;
	// Constructor
	Shader()
		: id(0) {
	}

	// NOLINT
	// Sets the current shader as active
	Shader& use();
	// Compiles the shader from given source code
	void compile(const GLchar* vertex_source, const GLchar* fragment_source, const GLchar* geometry_source = nullptr);
	// Note: geometry source code is optional
	// Utility functions
	void set_float(const GLchar* name, GLfloat value, GLboolean use_shader = false);
	void set_integer(const GLchar* name, GLint value, GLboolean use_shader = false);
	void set_vector2f(const GLchar* name, GLfloat x, GLfloat y, GLboolean use_shader = false);
	void set_vector2f(const GLchar* name, const glm::vec2& value, GLboolean use_shader = false);
	void set_vector3f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLboolean use_shader = false);
	void set_vector3f(const GLchar* name, const glm::vec3& value, GLboolean use_shader = false);
	void set_vector4f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean use_shader = false);
	void set_vector4f(const GLchar* name, const glm::vec4& value, GLboolean use_shader = false);
	void set_matrix4(const GLchar* name, const glm::mat4& matrix, GLboolean use_shader = false);
private:
	// Checks if compilation or linking failed and if so, print the error logs
	static void check_compile_errors(GLuint object, const std::string& type);
};


#endif //LEARNOPENGL_SHADER_H
