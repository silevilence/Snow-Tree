//
// Created by lenovo on 2019/3/5.
//

#include <glm/gtc/matrix_transform.hpp>
#include "MyTree.h"

Vertex *MyTree::_Circle_Vertices(const Point &point, const int &precision) {
    auto vertices = new Vertex[precision];

    glm::vec3 firstPoint = glm::vec3(1, 0, 0) * point.radius;
    auto firstPointV4 = glm::vec4(firstPoint, 1);
    for(int i = 0; i < precision; i++) {
        auto rotMat = glm::mat4(1);
        rotMat = glm::translate(rotMat, point.position);
        rotMat = glm::rotate(rotMat, glm::radians(360.f * i / precision), glm::vec3(0, 1, 0));
        rotMat = glm::rotate(rotMat, glm::radians(point.rotAngle), point.rotAxis);
        glm::vec4 vertex4 = rotMat * firstPointV4;
        vertices[i].position = glm::vec3(vertex4.x, vertex4.y, vertex4.z);
        vertices[i].normal = glm::normalize(vertices[i].position - point.position);
    }

    return vertices;
}

Mesh MyTree::Create_Cylinders(const Point *points, int pointNum, int precision) {
    Mesh mesh;

    auto vertices = new Vertex[pointNum * precision];
    for(int i = 0; i < pointNum; i++) {
        Vertex *single_vertices = _Circle_Vertices(points[i], precision);
        memcpy(vertices + i * precision, single_vertices, precision * sizeof(glm::vec3));
        delete single_vertices;
    }
    mesh.vertices = vertices;
    mesh.vertices_num = precision * pointNum;

    auto indices = new int[precision * 6 * (pointNum - 1)];
    for(int i = 0; i < pointNum - 1; i++) {
        int *triangles = _Create_Cylinder_Triangles(precision, i * precision, (i + 1) * precision);
        memcpy(indices + i * precision * 6, triangles, precision * 6 * sizeof(int));
        delete triangles;
    }
    mesh.indices = indices;
    mesh.indices_num = precision * 6 * (pointNum - 1);

    return mesh;
}

int *MyTree::_Create_Cylinder_Triangles(int precision, int bottom_start = -1, int top_start = -1) {
    // check params
    if(bottom_start < 0)
        bottom_start = 0;
    if(top_start < 0)
        top_start = precision;

    auto triangles = new int[precision * 6]; // 6 -> 2*3
    for(int i = 0; i < precision; i++) {
        triangles[i * 6] = triangles[i * 6 + 3] = i + bottom_start;
        triangles[i * 6 + 1] = triangles[i * 6 + 5] = (i + 1) % precision + top_start;
        triangles[i * 6 + 2] = i + top_start;
        triangles[i * 6 + 4] = (i + 1) % precision + bottom_start;
    }

    return triangles;
}
