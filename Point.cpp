//
// Created by lenovo on 2019/3/5.
//
#include "Point.h"


Point::Point(const glm::vec3 &position, GLfloat radius, const glm::vec3 &rotAxis, GLfloat rotAngle) {
    this->position = position;
    this->radius = radius;
    this->rotAxis = rotAxis;
    this->rotAngle = rotAngle;
}

Point::Point(const Point &point) {
    this->position = point.position;
    this->radius = point.radius;
    this->rotAxis = point.rotAxis;
    this->rotAngle = point.rotAngle;
}

Point &Point::operator=(const Point &point) = default;

Point::Point() = default;
