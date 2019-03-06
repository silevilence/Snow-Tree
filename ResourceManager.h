//
// Created by lenovo on 2017/9/13.
//

#ifndef LEARNOPENGL_RESOURCEMANAGER_H
#define LEARNOPENGL_RESOURCEMANAGER_H


#include <map>
#include <string>

#include <glad/glad.h>

#include "Texture2D.h"
#include "shader.h"


// A static singleton ResourceManager class that hosts several
// functions to load Textures and Shaders. Each loaded texture
// and/or shader is also stored for future reference by string
// handles. All functions and resources are static and no
// public constructor is defined.
class resource_manager {
public:
	// Resource storage
	static std::map<std::string, Shader> shaders;
	static std::map<std::string, texture2d> textures;

	// Loads (and generates) a shader program from file loading vertex, fragment (and geometry) shader's source code. If gShaderFile is not nullptr, it also loads a geometry shader
	static Shader load_shader(const GLchar* v_shader_file, const GLchar* f_shader_file, const GLchar* g_shader_file,
	                          std::string name);

	// Retrieves a stored sader
	static Shader& get_shader(std::string name);

	// Loads (and generates) a texture from file
	static texture2d load_texture(const GLchar* file, GLboolean alpha, std::string name);

	// Retrieves a stored texture
	static texture2d& get_texture(std::string name);

	// Properly de-allocates all loaded resources
	static void clear();

private:
	// Private constructor, that is we do not want any actual resource manager objects. Its members and functions should be publicly available (static).
	resource_manager() = default;

	// Loads and generates a shader from file
	static Shader load_shader_from_file(const GLchar* v_shader_file, const GLchar* f_shader_file,
	                                    const GLchar* g_shader_file = nullptr);

	// Loads a single texture from file
	static texture2d load_texture_from_file(const GLchar* file, GLboolean alpha);
};


#endif //LEARNOPENGL_RESOURCEMANAGER_H
