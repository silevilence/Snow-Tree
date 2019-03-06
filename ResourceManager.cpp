//
// Created by lenovo on 2017/9/13.
//

#include "ResourceManager.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include <stb_image.h>

// Instantiate static variables
std::map<std::string, texture2d> resource_manager::textures;
std::map<std::string, Shader> resource_manager::shaders;


Shader resource_manager::load_shader(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile,
                                     std::string name) {
	shaders[name] = load_shader_from_file(vShaderFile, fShaderFile, gShaderFile);
	return shaders[name];
}

Shader& resource_manager::get_shader(std::string name) {
	return shaders[name];
}

texture2d resource_manager::load_texture(const GLchar* file, GLboolean alpha, std::string name) {
	textures[name] = load_texture_from_file(file, alpha);
	return textures[name];
}

texture2d& resource_manager::get_texture(std::string name) {
	return textures[name];
}

void resource_manager::clear() {
	// (Properly) delete all shaders
	for (const auto iter : shaders) // NOLINT(performance-for-range-copy)
		glDeleteProgram(iter.second.id);
	// (Properly) delete all textures
	for (auto iter : textures) // NOLINT(performance-for-range-copy)
		glDeleteTextures(1, &iter.second.id);
}

Shader resource_manager::load_shader_from_file(const GLchar* vShaderFile, const GLchar* fShaderFile,
                                               const GLchar* gShaderFile) {
	// 1. Retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	try {
		// Open files
		std::ifstream vertexShaderFile(vShaderFile);
		std::ifstream fragmentShaderFile(fShaderFile);
		std::stringstream vShaderStream, fShaderStream;
		// Read file's buffer contents into streams
		vShaderStream << vertexShaderFile.rdbuf();
		fShaderStream << fragmentShaderFile.rdbuf();
		// close file handlers
		vertexShaderFile.close();
		fragmentShaderFile.close();
		// Convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		// If geometry shader path is present, also load a geometry shader
		if (gShaderFile != nullptr) {
			std::ifstream geometryShaderFile(gShaderFile);
			std::stringstream gShaderStream;
			gShaderStream << geometryShaderFile.rdbuf();
			geometryShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	} catch (std::exception e) { // NOLINT(cert-err09-cpp,cert-err61-cpp,misc-throw-by-value-catch-by-reference)
		std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
	}
	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();
	const GLchar* gShaderCode = geometryCode.c_str();
	// 2. Now create shader object from source code
	Shader shader;
	shader.compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
	return shader;
}

texture2d resource_manager::load_texture_from_file(const GLchar* file, GLboolean alpha) {
	// Create Texture object
	texture2d texture;
	if (alpha) {
		texture.internal_format = GL_RGBA;
		texture.image_format = GL_RGBA;
	}
	// Load image
	int width, height, nrChannels;
	unsigned char* image = stbi_load(file, &width, &height, &nrChannels, 0);
	// Now generate texture
	if (image) {
		texture.generate(width, height, image);
	} else {
		std::cout << "Failed to read texture files" << std::endl;
	}
	// And finally free image data
	stbi_image_free(image);
	return texture;
}
