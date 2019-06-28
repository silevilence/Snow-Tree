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
    glm::vec3 ori_pos;
    GLfloat radius{};
    glm::vec3 rotAxis;
    GLfloat rotAngle{};
    GLfloat ori_angle{};
    GLfloat E{};

    // constructors
    Point(const glm::vec3 &position, GLfloat radius, const glm::vec3 &rotAxis, GLfloat rotAngle, GLfloat E = 8.77e9);

    Point();

    Point(const Point &point);

    Point &operator=(const Point &);

    bool operator==(const Point &rhs) const;

    bool operator!=(const Point &rhs) const;
};

#endif //TREE_POINT_H
