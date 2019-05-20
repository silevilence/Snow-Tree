//
// Created by lenovo on 2019/3/14.
//

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "SimpleTreeBranch.h"
#include "MyTree.h"

float uniform_load_theta(const float &E, const float &I, const float &q, const float &L, const float &x);

float uniform_load_omega(const float &E, const float &I, const float &q, const float &L, const float &x);

SimpleTreeBranch::SimpleTreeBranch(const std::vector<Point> &points, const glm::vec3 &position, float rot_z,
                                   float rot_y, unsigned int precision, float length) : points(points),
                                                                                        precision(precision) {
    _transform = glm::translate(glm::mat4(1), position);
    _transform = glm::rotate(_transform, glm::radians(rot_y), glm::vec3(0, 1, 0));
    _transform = glm::rotate(_transform, glm::radians(rot_z), glm::vec3(0, 0, -1));

    this->b_theta = rot_z;
    this->length = length;

    this->generate_mesh();
}

SimpleTreeBranch::SimpleTreeBranch(const SimpleTreeBranch &branch) {
    _copy(branch);
}

SimpleTreeBranch::SimpleTreeBranch(SimpleTreeBranch &&branch) noexcept {
    _move(branch);
}

SimpleTreeBranch &SimpleTreeBranch::operator=(SimpleTreeBranch &&branch) noexcept {
    _move(branch);

    return *this;
}

void SimpleTreeBranch::generate_mesh(const bool &cal_normal) {
    this->mesh = MyTree::Create_Cylinders(&points[0], points.size(), precision);
    this->mesh.setup_mesh(cal_normal);
}

inline void SimpleTreeBranch::_copy(const SimpleTreeBranch &branch) {
    points = branch.points;
    mesh = branch.mesh;
//    position = branch.position;
//    rot_z = branch.rot_z;
//    rot_y = branch.rot_y;
    precision = branch.precision;
    _transform = branch._transform;
    b_theta = branch.b_theta;
    length = branch.length;
}

inline void SimpleTreeBranch::_move(SimpleTreeBranch &branch) {
    points = std::move(branch.points);
    mesh = std::move(branch.mesh);
//    position = branch.position;
//    rot_z = branch.rot_z;
//    rot_y = branch.rot_y;
    precision = branch.precision;
    _transform = branch._transform;
    b_theta = branch.b_theta;
    length = branch.length;
}

SimpleTreeBranch::SimpleTreeBranch(const Point *const points, unsigned int p_num, const glm::vec3 &position,
                                   float rot_z, float rot_y, unsigned int precision, float length) {
    new(this)SimpleTreeBranch(std::vector<Point>(points, points + p_num), position, rot_z, rot_y, precision, length);
//    this->generate_mesh();
}

void SimpleTreeBranch::draw(const glm::mat4 &transform, Shader shader) {
    shader.use();

//    glm::mat4 model = glm::translate(transform, position);
//    model = glm::rotate(model, glm::radians(rot_y), glm::vec3(0, 1, 0));
//    model = glm::rotate(model, glm::radians(rot_z), glm::vec3(0, 0, -1));

    shader.set_matrix4("model", transform * _transform);
    this->mesh.draw(shader);
}

SimpleTreeBranch::SimpleTreeBranch(const std::vector<Point> &points, const glm::mat4 &transform,
                                   unsigned int precision, float b_theta, float length) : points(points),
                                                                                          _transform(transform),
                                                                                          precision(precision),
                                                                                          b_theta(b_theta),
                                                                                          length(length) {
    this->generate_mesh();
}

void SimpleTreeBranch::update_points() {
//    auto *vertices = new Vertex[points.size() * precision];
    auto mesh = MyTree::Create_Cylinders(&points[0], points.size(), precision);
//    for(int i = 0; i < points.size(); i++) {
//        Vertex *single_vertices = MyTree::Circle_Vertices(points[i], precision);
//        memcpy(mesh.vertices + i * precision, single_vertices, precision * sizeof(Vertex));
//        delete single_vertices;
//    }
    memcpy(this->mesh.vertices, mesh.vertices, this->mesh.vertices_num * sizeof(Vertex));

    this->mesh.update_vertices_data();
}

bool SimpleTreeBranch::uniform_load_pressure(const float &q) {
//    bool stop = false;
    float threshold = fabsf(length * cosf(glm::radians(b_theta)));
    if(threshold < 1e-5) {
        threshold = length;
    }

    for(int i = 0; i < points.size(); ++i) {
        const float I = glm::pi<float>() * powf(this->points[i].radius, 4.f) / 4;
        const float x = length - i * length / points.size() - 1;

        float q_vert = q * sinf(glm::radians(b_theta));
//                    float q_hori = q * cosf(glm::radians(b_theta));
        float omega = uniform_load_omega(this->points[i].E, I, q_vert, length, x);
        float theta = uniform_load_theta(this->points[i].E, I, q_vert, length, x);

        if(fabsf(omega) >= threshold) {
            return true;
//                        std::cout << "stop" << std::endl;
        }

//                if(i == SEG)
//                    std::cout << omega << std::endl;

        this->points[i].position = glm::vec3(-omega, this->points[i].position.y, this->points[i].position.z);
        this->points[i].rotAngle = -theta;

//                if(i == 0)
//                    std::cout << omega << ' ' << theta << std::endl;
    }

    this->update_points();
    return false;
}

SimpleTreeBranch &SimpleTreeBranch::operator=(const SimpleTreeBranch &branch) = default;

SimpleTreeBranch::~SimpleTreeBranch() = default;

SimpleTreeBranch::SimpleTreeBranch() = default;

inline float uniform_load_theta(const float &E, const float &I, const float &q, const float &L, const float &x) {
    return q * (powf(L, 3.f) - powf(x, 3.f)) / (6 * E * I);
}

inline float uniform_load_omega(const float &E, const float &I, const float &q, const float &L, const float &x) {
    return q * (4 * powf(L, 3.f) * x - powf(x, 4.f) - 3 * powf(L, 4.f)) / (24 * E * I);
}
