//
// Created by lenovo on 2019/3/6.
//

#ifndef TREE_MESH_H
#define TREE_MESH_H

#include <glad/glad.h>
#include "Vertex.h"
#include "Shader.h"

class Mesh {
public:
    Vertex *vertices;
    int vertices_num;
    int *indices;
    int indices_num;

    Mesh();

    void setup_mesh();
    void draw(Shader shader);
private:
    GLuint VAO, VBO, EBO;
};

#endif //TREE_MESH_H
