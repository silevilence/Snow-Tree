//
// Created by lenovo on 2019/8/17.
//

#include "SnowParticle.h"

Point *SnowParticle::generate_particle_points(const float &radius, const int &seg_num) {
    assert(radius > 0 and seg_num > 0);
    auto points = new Point[seg_num * 2 + 1];

    // 上半球
    auto distance_delta = radius / seg_num;
    for(int seg_id = 0; seg_id < seg_num; seg_id++) {

        Point point;

        float x = radius - seg_id * distance_delta;
        point.position = glm::vec3(0, x, 0);
        point.ori_pos = point.position;
        point.radius = sqrtf(radius * radius - x * x);

        points[seg_id] = point;
    }

    // 圆心
    points[seg_num] = Point();
    points[seg_num].position = glm::vec3(0, 0, 0);
    points[seg_num].ori_pos = glm::vec3(0, 0, 0);
    points[seg_num].radius = radius;

    // 下半球
    for(int seg_id = 1; seg_id <= seg_num; seg_id++) {

        Point point;

        float x = -seg_id * distance_delta;
        point.position = glm::vec3(0, x, 0);
        point.ori_pos = point.position;
        point.radius = sqrtf(radius * radius - x * x);

        points[seg_id + seg_num] = point;
    }

    return points;
}

void SnowParticle::generate_mesh() {
    this->mesh = Create_Cylinders(generate_particle_points(this->radius, this->seg_num),
                                  this->seg_num * 2 + 1, this->seg_num, false, false);
    this->mesh.setup_mesh();
}

void SnowParticle::draw(const glm::mat4 &transform, Shader shader) {
    shader.use();

//    glm::mat4 model = glm::translate(transform, position);
//    model = glm::rotate(model, glm::radians(rot_y), glm::vec3(0, 1, 0));
//    model = glm::rotate(model, glm::radians(rot_z), glm::vec3(0, 0, -1));

    shader.set_matrix4("model", transform);
    this->mesh.draw(shader);
}
