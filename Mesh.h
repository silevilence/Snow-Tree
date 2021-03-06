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

    Mesh(Mesh &&mesh) noexcept;

    Mesh(const Mesh &mesh) noexcept;

    ~Mesh();

    Mesh &operator=(Mesh &&other) noexcept;

    Mesh &operator=(const Mesh &mesh) noexcept;

    void setup_mesh(const bool &recalculate_normal = true, const bool &force = false, const bool &texture = false);

    void update_vertices_data(const bool &recalculate_normal = true);

    void draw(Shader shader) const;

private:
    GLuint VAO, VBO, EBO;
    bool is_set;

    void _clear_buffer();

    void _recalculate_normal();

    void _move(Mesh &mesh);

    void _copy(const Mesh &mesh);
};

#endif //TREE_MESH_H
