//
// Created by lenovo on 2017/9/13.
//
#include <iostream>

#include "texture2d.h"


texture2d::texture2d()
        : width(0), height(0), internal_format(GL_RGB), image_format(GL_RGB), wrap_s(GL_MIRRORED_REPEAT),
          wrap_t(GL_MIRRORED_REPEAT), filter_min(GL_LINEAR), filter_max(GL_LINEAR) {
    glGenTextures(1, &this->id);
}

void texture2d::generate(const GLuint width, const GLuint height, unsigned char *data) {
    this->width = width;
    this->height = height;
    // Create Texture
    glBindTexture(GL_TEXTURE_2D, this->id);
    glTexImage2D(GL_TEXTURE_2D, 0, this->internal_format, width, height, 0, this->image_format,
                 GL_UNSIGNED_BYTE, data);
    // Set Texture wrap and filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->wrap_t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->filter_min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->filter_max);
    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

void texture2d::bind() const {
    glBindTexture(GL_TEXTURE_2D, this->id);
}
