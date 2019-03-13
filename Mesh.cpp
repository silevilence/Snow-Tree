//
// Created by lenovo on 2019/3/6.
//

#include "Mesh.h"

Mesh::Mesh() {
    vertices = nullptr;
    indices = nullptr;
    is_set = false;
}

void Mesh::setup_mesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices_num * sizeof(Vertex), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_num * sizeof(GLuint), indices, GL_STATIC_DRAW);

    // 顶点位置
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    // 顶点法线
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));
    // 顶点纹理坐标
//    glEnableVertexAttribArray(2);
//    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, TexCoords));

    glBindVertexArray(0);

    is_set = true;
}

void Mesh::draw(Shader shader) {
    shader.use();
//    unsigned int diffuseNr = 1;
//    unsigned int specularNr = 1;
//    for(unsigned int i = 0; i < textures.size(); i++) {
//        glActiveTexture(GL_TEXTURE0 + i); // 在绑定之前激活相应的纹理单元
//        // 获取纹理序号（diffuse_textureN 中的 N）
//        string number;
//        string name = textures[i].type;
//        if(name == "texture_diffuse")
//            number = std::to_string(diffuseNr++);
//        else if(name == "texture_specular")
//            number = std::to_string(specularNr++);
//
//        shader.set_float(("material." + name + number).c_str(), i);
//        glBindTexture(GL_TEXTURE_2D, textures[i].id);
//    }
//    glActiveTexture(GL_TEXTURE0);

    // 绘制网格
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices_num, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

Mesh::~Mesh() {
    delete vertices;
    delete indices;
    if(is_set) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
}

Mesh &Mesh::operator=(Mesh &&other) noexcept {
    if(this != &other) {
        this->~Mesh();
        this->is_set = false;

        this->vertices = other.vertices;
        this->vertices_num = other.vertices_num;
        other.vertices = nullptr;

        this->indices = other.indices;
        this->indices_num = other.indices_num;
        other.indices = nullptr;
    }
    return *this;
}

Mesh::Mesh(Mesh &&mesh) noexcept {
    this->~Mesh();
    this->is_set = false;

    this->vertices = mesh.vertices;
    this->vertices_num = mesh.vertices_num;
    mesh.vertices = nullptr;

    this->indices = mesh.indices;
    this->indices_num = mesh.indices_num;
    mesh.indices = nullptr;
}

Mesh::Mesh(const Mesh &mesh) noexcept {
    this->~Mesh();
    this->is_set = false;

    vertices_num = mesh.vertices_num;
    vertices = new Vertex[vertices_num];
    memcpy(vertices, mesh.vertices, vertices_num * sizeof(Vertex));

    indices_num = mesh.indices_num;
    indices = new int[indices_num];
    memcpy(indices, mesh.indices, indices_num * sizeof(int));
}
