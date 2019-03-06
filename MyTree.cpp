//
// Created by lenovo on 2019/3/5.
//

#include <glm/gtc/matrix_transform.hpp>
#include "MyTree.h"

glm::vec3 *MyTree::_Circle_Vertices(const Point &point, const int &precision) {
    auto vertices = new glm::vec3[precision];

    glm::vec3 firstPoint = glm::vec3(1, 0, 0) * point.radius;
    auto firstPointV4 = glm::vec4(firstPoint, 1);
    for(int i = 0; i < precision; i++) {
        auto rotMat = glm::mat4(1);
        rotMat = glm::translate(rotMat, point.position);
        rotMat = glm::rotate(rotMat, glm::radians(360.f * i / precision), glm::vec3(0, 1, 0));
        rotMat = glm::rotate(rotMat, glm::radians(point.rotAngle), point.rotAxis);
        glm::vec4 vertex4 = rotMat * firstPointV4;
        vertices[i] = glm::vec3(vertex4.x, vertex4.y, vertex4.z);
    }

    return vertices;
}
