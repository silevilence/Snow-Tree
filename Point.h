//
// Created by lenovo on 2019/3/5.
//

#ifndef TREE_POINT_H
#define TREE_POINT_H

#include <glm/vec3.hpp>
#include <glad/glad.h>

class Point {
public:
    glm::vec3 position;
    GLfloat radius;
    glm::vec3 rotAxis;
    GLfloat rotAngle;

    // constructors
    Point(const glm::vec3 &position, GLfloat radius, const glm::vec3 &rotAxis, GLfloat rotAngle);

    Point();

    Point(const Point &point);

    Point &operator=(const Point &) = delete;
};

#endif //TREE_POINT_H
