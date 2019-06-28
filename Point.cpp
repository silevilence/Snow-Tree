//
// Created by lenovo on 2019/3/5.
//
#include "Point.h"


Point::Point(const glm::vec3 &position, GLfloat radius, const glm::vec3 &rotAxis, GLfloat rotAngle, GLfloat E) {
    this->position = position;
    this->ori_pos = position;
    this->radius = radius;
    this->rotAxis = rotAxis;
    this->rotAngle = rotAngle;
    this->ori_angle = rotAngle;
    this->E = E;
}

Point::Point(const Point &point) {
    this->position = point.position;
    this->ori_pos = point.ori_pos;
    this->radius = point.radius;
    this->rotAxis = point.rotAxis;
    this->rotAngle = point.rotAngle;
    this->ori_angle = point.ori_angle;
    this->E = point.E;
}

bool Point::operator==(const Point &rhs) const {
    return position == rhs.position &&
           ori_pos == rhs.ori_pos &&
           radius == rhs.radius &&
           rotAxis == rhs.rotAxis &&
           rotAngle == rhs.rotAngle &&
           ori_angle == rhs.ori_angle &&
           E == rhs.E;
}

bool Point::operator!=(const Point &rhs) const {
    return !(rhs == *this);
}

Point &Point::operator=(const Point &point) = default;

Point::Point() = default;
