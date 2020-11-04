#include <utility>

//
// Created by lenovo on 2019/3/14.
//

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "SimpleTreeBranch.h"
#include "MyTree.h"
#include "BoundBox.h"

float uniform_load_theta(const float &E, const float &I, const float &q, const float &L, const float &x);

float uniform_load_omega(const float &E, const float &I, const float &q, const float &L, const float &x);

float concentrated_load_theta(const float &E, const float &I, const float &F, const float &L, const float &x);

float concentrated_load_omega(const float &E, const float &I, const float &F, const float &L, const float &x);

const float RADIUS_THRESHOLD = 0.1f;

SimpleTreeBranch::SimpleTreeBranch(std::vector<Point> points, const glm::vec3 &position, float rot_z,
                                   float rot_y, unsigned int precision, float length)
        : points(std::move(points)), precision(precision), rot_y(rot_y), rot_z(rot_z) {
    _transform = glm::translate(glm::mat4(1), position);
    _transform = glm::rotate(_transform, glm::radians(rot_y), glm::vec3(0, 1, 0));
    _transform = glm::rotate(_transform, glm::radians(rot_z), glm::vec3(0, 0, -1));

    this->b_theta = rot_z;
    this->length = length;

    changed = false;

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
    rot_z = branch.rot_z;
    rot_y = branch.rot_y;
    precision = branch.precision;
    _transform = branch._transform;
    b_theta = branch.b_theta;
    length = branch.length;

    parent = branch.parent;
    children = branch.children;
}

inline void SimpleTreeBranch::_move(SimpleTreeBranch &branch) {
    points = std::move(branch.points);
    mesh = std::move(branch.mesh);
//    position = branch.position;
    rot_z = branch.rot_z;
    rot_y = branch.rot_y;
    precision = branch.precision;
    _transform = branch._transform;
    b_theta = branch.b_theta;
    length = branch.length;

    parent = branch.parent;
    children = std::move(branch.children);
}

SimpleTreeBranch::SimpleTreeBranch(const Point *const points, unsigned int p_num, const glm::vec3 &position,
                                   float rot_z, float rot_y, unsigned int precision, float length) {
    new(this)SimpleTreeBranch(std::vector<Point>(points, points + p_num), position, rot_z, rot_y, precision,
                              length);
//    this->generate_mesh();
}

void SimpleTreeBranch::draw(const glm::mat4 &transform, Shader shader) const {
    shader.use();

//    glm::mat4 model = glm::translate(transform, position);
//    model = glm::rotate(model, glm::radians(rot_y), glm::vec3(0, 1, 0));
//    model = glm::rotate(model, glm::radians(rot_z), glm::vec3(0, 0, -1));

    *last_transform = transform * _transform;
    shader.set_matrix4("model", *last_transform);
    this->mesh.draw(shader);

    // draw children
    if(!children.empty()) {
        Point point = this->points[this->points.size() - 1];
        auto model = glm::translate(glm::mat4(1.0f), point.position);
        model = glm::rotate(model, point.rotAngle, point.rotAxis);

//        model = _transform * model * transform;
        model = transform * _transform * model;
        for(const auto &branch_ptr : children) {
            branch_ptr->draw(model, shader);
        }
    }
}

SimpleTreeBranch::SimpleTreeBranch(std::vector<Point> points, const glm::mat4 &transform,
                                   unsigned int precision, float b_theta, float length)
        : points(std::move(points)), _transform(transform), precision(precision), b_theta(b_theta),
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
    return uniform_load_pressure(q, b_theta);
}

bool SimpleTreeBranch::uniform_load_pressure(const float &q, float q_theta) {
    while(q_theta > 180)
        q_theta -= 360;
    while(q_theta < -180)
        q_theta += 360;

    bool stop = false;
    float threshold = fabsf(length * cosf(glm::radians(q_theta)));
    if(threshold < 1e-5) {
        threshold = length;
    }
    threshold = length / 50;

    for(int i = 0; i < points.size(); ++i) {
        const float r =
                this->points[i].radius < RADIUS_THRESHOLD ? RADIUS_THRESHOLD : this->points[i].radius;
        const float I = glm::pi<float>() * powf(r, 4.f) / 4;
        const float x = length - i * length / points.size() - 1;

        float q_vert = q * sinf(glm::radians(q_theta));
//                    float q_hori = q * cosf(glm::radians(q_theta));
        float omega = uniform_load_omega(this->points[i].E, I, q_vert, length, x);
        float theta = uniform_load_theta(this->points[i].E, I, q_vert, length, x);

        if(fabsf(omega) >= threshold) {
            stop = true;
//                        std::cout << "stop" << std::endl;
        }

//                if(i == SEG)
//                    std::cout << omega << std::endl;

        this->points[i].position += glm::vec3(-omega, 0, 0);
        this->points[i].rotAngle += -theta;

//                if(i == 0)
//                    std::cout << omega << ' ' << q_theta << std::endl;
    }

    if(fabsf(this->points.back().position.x - this->points.back().ori_pos.x) >= threshold) {
        stop = true;
    }
//    if(q > 10000)
//        stop = true;

    if(stop and this->parent != nullptr) {
        auto parent = this->parent;
        this->parent = nullptr;

        for(auto ptr = parent->children.begin(); ptr != parent->children.end(); ptr++) {
            if(*ptr == this) {
                parent->children.erase(ptr);
                break;
            }
        }
    }

//    this->update_points();

    // 力传导
    if(parent != nullptr) {
        auto ps = points.size() - 1;
        auto force = points[ps].position - points[0].position;
        auto angle = fabsf(glm::degrees(atanf(force.y / force.x)) + b_theta);
        stop = parent->concentrated_load_pressure(q * length, angle) or stop;
//        parent->add_concentrated_force(q * length, angle);
    }

    changed = true;

    return stop;
}

void SimpleTreeBranch::add_child(SimpleTreeBranch &branch) {
    SimpleTreeBranch *ptr = &branch;
    children.push_back(ptr);
    branch.parent = this;
}

bool SimpleTreeBranch::concentrated_load_pressure(const float &F, float f_theta) {
    while(f_theta > 180)
        f_theta -= 360;
    while(f_theta < -180)
        f_theta += 360;

    bool stop = false;
    float threshold = fabsf(length * cosf(glm::radians(f_theta)));
    if(threshold < 1e-5) {
        threshold = length;
    }

    for(int i = 0; i < points.size(); ++i) {
        const float r =
                this->points[i].radius < RADIUS_THRESHOLD ? RADIUS_THRESHOLD : this->points[i].radius;
        const float I = glm::pi<float>() * powf(r, 4.f) / 4;
        const float x = length - i * length / points.size() - 1;

        float f_vert = F * sinf(glm::radians(f_theta));
        float omega = concentrated_load_omega(this->points[i].E, I, f_vert, length, x);
        float theta = concentrated_load_theta(this->points[i].E, I, f_vert, length, x);

        if(fabsf(omega) >= threshold) {
            stop = true;
        }

        this->points[i].position += glm::vec3(-omega, 0, 0);
        this->points[i].rotAngle += -theta;
    }

//    this->update_points();

    // 力传导
    if(parent != nullptr) {
        auto ps = points.size() - 1;
        auto force = points[ps].position - points[0].position;
        auto angle = fabsf(glm::degrees(atanf(force.y / force.x)) + b_theta);
        stop = parent->concentrated_load_pressure(F, angle) or stop;
//        parent->add_concentrated_force(F, angle);
    }

    changed = true;

    return stop;
}

void SimpleTreeBranch::reset(const bool &recursive) {
    for(auto &point : points) {
        point.position = point.ori_pos;
        point.rotAngle = point.ori_angle;
    }

    if(changed)
        update_points();
    force_sum = glm::vec3(0);
    changed = false;

    if(recursive and (not children.empty())) {
        for(auto &child : children) {
            child->reset(true);
        }
    }
}

bool SimpleTreeBranch::complete_calculate(const bool &recursive) {
    bool stop = false;

//    if(force_sum.x != 0 or force_sum.y != 0) {
//        float F = sqrtf(powf(force_sum.x, 2) + powf(force_sum.y, 2));
//        float angle = acosf(force_sum.y / F);
//
//        stop = this->concentrated_load_pressure(F, angle) or stop;
//    }

    if(changed)
        this->update_points();
//    changed = false;

    if(recursive and (not children.empty())) {
        for(auto &child : children) {
            stop = child->complete_calculate(true) or stop;
        }
    }

    return stop;
}

void SimpleTreeBranch::add_concentrated_force(const float &F, float f_theta) {
    this->force_sum += glm::vec3(sinf(f_theta), cosf(f_theta), 0) * F;
}

BoundBox *SimpleTreeBranch::create_bound_box() {
    auto *tbox = new BoundBox();
    tbox->branch = this;
    Vertex *vs = this->mesh.vertices;

    tbox->xmin = tbox->xmax = vs->position.x;
    tbox->ymin = tbox->ymax = vs->position.y;
    tbox->zmin = tbox->zmax = vs->position.z;

    for(int i = 1; i < this->mesh.vertices_num; ++i) {
        glm::vec3 &p = vs[i].position;
        if(p.x < tbox->xmin)
            tbox->xmin = p.x;
        if(p.x > tbox->xmax)
            tbox->xmax = p.x;
        if(p.y < tbox->ymin)
            tbox->ymin = p.y;
        if(p.y > tbox->ymax)
            tbox->ymax = p.y;
        if(p.z < tbox->zmin)
            tbox->zmin = p.z;
        if(p.z > tbox->zmax)
            tbox->zmax = p.z;
    }

    return tbox;
}

SimpleTreeBranch &SimpleTreeBranch::operator=(const SimpleTreeBranch &branch) = default;

SimpleTreeBranch::~SimpleTreeBranch() = default;

SimpleTreeBranch::SimpleTreeBranch() = default;

inline float
uniform_load_theta(const float &E, const float &I, const float &q, const float &L, const float &x) {
    return q * (powf(L, 3.f) - powf(x, 3.f)) / (6 * E * I);
}

inline float
uniform_load_omega(const float &E, const float &I, const float &q, const float &L, const float &x) {
    return q * (4 * powf(L, 3.f) * x - powf(x, 4.f) - 3 * powf(L, 4.f)) / (24 * E * I);
}

inline float
concentrated_load_theta(const float &E, const float &I, const float &F, const float &L, const float &x) {
    return F * L * (L - x) / (E * I);
}

inline float
concentrated_load_omega(const float &E, const float &I, const float &F, const float &L, const float &x) {
    return -F * L * powf(x - L, 2.f) / (2 * E * I);
}
