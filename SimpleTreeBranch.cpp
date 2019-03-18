//
// Created by lenovo on 2019/3/14.
//

#include <glm/gtc/matrix_transform.hpp>
#include "SimpleTreeBranch.h"
#include "MyTree.h"

SimpleTreeBranch::SimpleTreeBranch(const std::vector<Point> &points, const glm::vec3 &position, float rot_z,
                                   float rot_y, unsigned int precision) : points(points), position(position),
                                                                          rot_z(rot_z), rot_y(rot_y),
                                                                          precision(precision) {
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

void SimpleTreeBranch::_copy(const SimpleTreeBranch &branch) {
    points = branch.points;
    mesh = branch.mesh;
    position = branch.position;
    rot_z = branch.rot_z;
    rot_y = branch.rot_y;
    precision = branch.precision;
}

void SimpleTreeBranch::_move(SimpleTreeBranch &branch) {
    points = std::move(branch.points);
    mesh = std::move(branch.mesh);
    position = branch.position;
    rot_z = branch.rot_z;
    rot_y = branch.rot_y;
    precision = branch.precision;
}

SimpleTreeBranch::SimpleTreeBranch(const Point *const points, unsigned int p_num, const glm::vec3 &position,
                                   float rot_z, float rot_y, unsigned int precision) : position(position), rot_z(rot_z),
                                                                                       rot_y(rot_y),
                                                                                       precision(precision) {
    this->points = std::vector<Point>(points, points + p_num);
    this->generate_mesh();
}

void SimpleTreeBranch::draw(const glm::mat4 &transform, Shader shader) {
    shader.use();

    glm::mat4 model = glm::translate(transform, position);
    model = glm::rotate(model, glm::radians(rot_y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(rot_z), glm::vec3(0, 0, -1));

    shader.set_matrix4("model", model);
    this->mesh.draw(shader);
}

SimpleTreeBranch &SimpleTreeBranch::operator=(const SimpleTreeBranch &branch) = default;

SimpleTreeBranch::~SimpleTreeBranch() = default;

SimpleTreeBranch::SimpleTreeBranch() = default;
