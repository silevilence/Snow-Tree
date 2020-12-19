//
// Created by lenovo on 2019/3/6.
//

#include <map>
#include <vector>
#include "Mesh.h"

Mesh::Mesh() {
    vertices = nullptr;
    indices = nullptr;
    is_set = false;
}

void Mesh::setup_mesh(const bool &recalculate_normal, const bool &force, const bool &texture) {
    if(force) {
        _clear_buffer();
    } else if(is_set) {
        return;
    }

    if(recalculate_normal)
        _recalculate_normal();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices_num * sizeof(Vertex), vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_num * sizeof(GLuint), indices, GL_STATIC_DRAW);

    // 顶点位置
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    // 顶点法线
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));
    // 顶点纹理坐标
    if(texture) {
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, tex_coord));
    }

    glBindVertexArray(0);

    is_set = true;
}

void Mesh::_clear_buffer() {
    if(is_set) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
    is_set = false;
}

void Mesh::draw(Shader shader) const {
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
    _clear_buffer();
}

Mesh &Mesh::operator=(Mesh &&other) noexcept {
    if(this != &other) {
        this->~Mesh();

        _move(other);
    }

    return *this;
}

Mesh::Mesh(Mesh &&mesh) noexcept {
    this->~Mesh();

    _move(mesh);
}

Mesh::Mesh(const Mesh &mesh) noexcept {
    this->~Mesh();

    _copy(mesh);
}

void Mesh::_recalculate_normal() {
    // 从面片编号到面法向量的映射
    std::map<int, glm::vec3> triangles_normal;
    for(int i = 0; i < vertices_num; i++) {
        // 首先取出含有该点的所有面
        std::vector<int> triangles_index;
        for(int j = 0; j < indices_num; j++) {
            if(indices[j] == i) {
                triangles_index.emplace_back(j / 3);
            }
        }

        glm::vec3 normal = glm::vec3(0);
        // 计算并累加面的法向量
        for(auto index: triangles_index) {
            // 若有，直接取出
            if(triangles_normal.count(index) != 0) {
                normal += triangles_normal[index];
            } else {
                glm::vec3 p_a = vertices[indices[index * 3]].position,
                        p_b = vertices[indices[index * 3 + 1]].position,
                        p_c = vertices[indices[index * 3 + 2]].position;
                if(p_a == p_b or p_b == p_c) {
                    triangles_normal[index] = glm::vec3(0);
                    continue;
                }
                glm::vec3 &&p_ab = p_b - p_a,
                        &&p_bc = p_c - p_b;
                glm::vec3 &&tri_normal = glm::normalize(glm::cross(p_ab, p_bc));

                normal += tri_normal;
                triangles_normal[index] = tri_normal;
            }
        }

        // 单位化
        vertices[i].normal = glm::normalize(normal);
    }
}

Mesh &Mesh::operator=(const Mesh &mesh) noexcept {
    this->~Mesh();

    _copy(mesh);

    return *this;
}

inline void Mesh::_copy(const Mesh &mesh) {
    vertices_num = mesh.vertices_num;
    vertices = new Vertex[vertices_num];
    memcpy(vertices, mesh.vertices, vertices_num * sizeof(Vertex));

    indices_num = mesh.indices_num;
    indices = new int[indices_num];
    memcpy(indices, mesh.indices, indices_num * sizeof(int));

    this->setup_mesh();
}

inline void Mesh::_move(Mesh &mesh) {
    this->vertices = mesh.vertices;
    this->vertices_num = mesh.vertices_num;
    mesh.vertices = nullptr;

    this->indices = mesh.indices;
    this->indices_num = mesh.indices_num;
    mesh.indices = nullptr;

    this->VAO = mesh.VAO;
    this->VBO = mesh.VBO;
    this->EBO = mesh.EBO;
    this->is_set = mesh.is_set;
    mesh.is_set = false;
}

void Mesh::update_vertices_data(const bool &recalculate_normal) {
    if(not is_set)
        return;

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    void *p_vbo = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    memcpy(p_vbo, vertices, vertices_num * sizeof(Vertex));
    glUnmapBuffer(GL_ARRAY_BUFFER);

    if(recalculate_normal)
        this->_recalculate_normal();
}
