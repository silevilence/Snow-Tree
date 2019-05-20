//
// Created by lenovo on 2019/3/5.
//

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "MyTree.h"

Vertex *MyTree::Circle_Vertices(const Point &point, const int &precision) {
    auto vertices = new Vertex[precision];

    glm::vec3 firstPoint = glm::vec3(point.radius, 0, 0);
    auto firstPointV4 = glm::vec4(firstPoint, 1);
    for(int i = 0; i < precision; i++) {
        auto rotMat = glm::mat4(1);
        rotMat = glm::translate(rotMat, point.position);
        if(glm::vec3(0) != point.rotAxis)
            rotMat = glm::rotate(rotMat, glm::radians(point.rotAngle), point.rotAxis);
        rotMat = glm::rotate(rotMat, glm::radians(360.f * i / precision), glm::vec3(0, 1, 0));
        glm::vec4 vertex4 = rotMat * firstPointV4;
        vertices[i].position = glm::vec3(vertex4.x, vertex4.y, vertex4.z);
        vertices[i].normal = glm::normalize(vertices[i].position - point.position);
    }

    return vertices;
}

Mesh MyTree::Create_Cylinders(const Point *points, int pointNum, int precision, const bool &close_bottom,
                              const bool &close_top) {
    Mesh mesh;

    auto vertices = new Vertex[pointNum * precision];
    for(int i = 0; i < pointNum; i++) {
        Vertex *single_vertices = Circle_Vertices(points[i], precision);
        memcpy(vertices + i * precision, single_vertices, precision * sizeof(Vertex));
        delete single_vertices;
    }
    mesh.vertices = vertices;
    mesh.vertices_num = precision * pointNum;

    int triangles_num = precision * 2 * (pointNum - 1);
    if(close_bottom)
        triangles_num += precision - 2;
    if(close_top)
        triangles_num += precision - 2;
    auto indices = new int[triangles_num * 3];
    int offset = 0;
    for(int i = 0; i < pointNum - 1; i++) {
        bool cb = i == 0 ? close_bottom : false;
        bool ct = i == pointNum - 2 ? close_top : false;

        int t_size = precision * 2;
        if(cb)
            t_size += precision - 2;
        if(ct)
            t_size += precision - 2;

        int *triangles = _Create_Cylinder_Triangles(precision, i * precision, (i + 1) * precision, cb, ct);
        memcpy(indices + offset, triangles, t_size * 3 * sizeof(int));
        delete triangles;

        offset += t_size * 3;
    }
    mesh.indices = indices;
    mesh.indices_num = triangles_num * 3;

    return mesh;
}

int *
MyTree::_Create_Cylinder_Triangles(int precision, int bottom_start = -1, int top_start = -1, const bool &close_bottom,
                                   const bool &close_top) {
    // check params
    if(bottom_start < 0)
        bottom_start = 0;
    if(top_start < 0)
        top_start = precision;

    int triangles_num = precision * 2;
    if(precision > 2) {
        if(close_bottom)
            triangles_num += precision - 2;
        if(close_top)
            triangles_num += precision - 2;
    }
    auto triangles = new int[triangles_num * 3];
    for(int i = 0; i < precision; i++) {
        triangles[i * 6] = triangles[i * 6 + 3] = i + bottom_start;
        triangles[i * 6 + 1] = i + top_start;
        triangles[i * 6 + 2] = triangles[i * 6 + 4] = (i + 1) % precision + top_start;
        triangles[i * 6 + 5] = (i + 1) % precision + bottom_start;
    }

    // close bottom or top
    if(precision > 2) {
        int index = precision * 6;
        if(close_bottom) {
            for(int i = 0; i < precision - 2; i++) {
                triangles[index] = bottom_start;
                triangles[index + 1] = (i + 2) % precision + bottom_start;
                triangles[index + 2] = (i + 1) % precision + bottom_start;
                index += 3;
            }
        }
        if(close_top) {
            for(int i = 0; i < precision - 2; i++) {
                triangles[index] = top_start;
                triangles[index + 1] = (i + 2) % precision + top_start;
                triangles[index + 2] = (i + 1) % precision + top_start;
                index += 3;
            }
        }
    }

    return triangles;
}

Point *MyTree::generate_circular_helix(const GLfloat &a, const GLfloat &omega, const GLfloat &H, const GLfloat &radius,
                                       int times = -1) {
    assert(a > 0 and omega > 0 and H > 0 and radius > 0);
    if(times < 0) {
        times = (int) omega / 360;
    }

    auto points = new Point[times];
    auto angle = (float) glm::degrees(atan2(H, 2 * glm::pi<GLfloat>() * a));
    angle = 90 - angle;

    for(int i = 0; i < times; i++) {
        GLfloat degree = omega * i;
        Point point;

        point.radius = radius;

        // position
        auto trans_mat = glm::mat4(1);
        trans_mat = glm::translate(trans_mat, glm::vec3(0, degree / 360 * H, 0));
        trans_mat = glm::rotate(trans_mat, glm::radians(degree), glm::vec3(0, 1, 0));

        auto pos = glm::vec4(a, 0, 0, 1);
        pos = trans_mat * pos;
        point.position = glm::vec3(pos.x, pos.y, pos.z);

        point.rotAxis = glm::vec3(-pos.x, 0, -pos.z);
        point.rotAngle = angle;

        points[i] = point;
    }
    return points;
}

Point *MyTree::generate_branch(const float &length, const glm::vec3 &rot_axis, const float &start_angel,
                               const float &start_radius, const float &end_radius, const int &seg_num,
                               const float &curve_angle, const float &exp, const float &base_e, const float &s_min,
                               const float &s_max, const float &epsilon3) {
    assert(length > 0 and start_radius >= end_radius and end_radius >= 0 and seg_num > 0 and exp > 0);
    auto points = new Point[seg_num + 1];

    auto mat = glm::mat4(1);
    float x = 0;
    for(int seg_id = 1; seg_id <= seg_num; seg_id++) {
        auto distance_delta = (glm::pow((float) seg_id / seg_num, exp) - glm::pow((float) (seg_id - 1) / seg_num, exp))
                              * length;
        x += distance_delta;
//        std::cout << x << std::endl;

        float ti = powf(1 - x / length, epsilon3);

        // 在变换矩阵右边不停乘上新的变换，达到累加变换的效果
        if(rot_axis != glm::vec3(0)) {
            if(seg_id == 1) {
                mat = glm::rotate(mat, glm::radians(start_angel), rot_axis);
            } else {
                mat = glm::rotate(mat, glm::radians(curve_angle), rot_axis);
            }
        }
        mat = glm::translate(mat, glm::vec3(0, distance_delta, 0));

        Point point;

        auto pos = mat * glm::vec4(0, 0, 0, 1);
        point.position = glm::vec3(pos.x, pos.y, pos.z);

        point.radius = (seg_num - seg_id) * (start_radius - end_radius) / seg_num + end_radius;


        auto rotAxis = glm::normalize(glm::cross(glm::vec3(0, 1, 0), point.position));
        auto angle = acosf(glm::dot(glm::vec3(0, 1, 0), point.position) / glm::sqrt(
                point.position.x * point.position.x + point.position.y * point.position.y +
                point.position.z * point.position.z));
        point.rotAngle = start_angel + curve_angle * seg_id;
        point.rotAxis = glm::vec3(0, 0, 1);
        point.E = base_e * ((1 - ti) * s_min + ti * s_max);

//        std::cout << point.E << std::endl;

        points[seg_id] = point;
    }

    Point point;
    point.position = glm::vec3(0);
    point.radius = start_radius;
    point.rotAxis = points[1].rotAxis;
//    point.rotAxis = glm::vec3(0, 0, -1);
    point.rotAngle = start_angel;
    point.E = base_e * s_max;
    points[0] = point;

    return points;
}

Point *MyTree::generate_branch(const float &length, const glm::vec3 &start_direction, const float &start_radius,
                               const float &end_radius, const int &seg_num, const float &curve_angle,
                               const float &exp) {
    assert(start_direction != glm::vec3(0) and start_direction != glm::vec3(0, 1, 0));

    auto rot_axis = glm::normalize(glm::cross(glm::vec3(0, 1, 0), start_direction));
    auto angle = acosf(glm::dot(glm::vec3(0, 1, 0), start_direction) / glm::sqrt(
            start_direction.x * start_direction.x + start_direction.y * start_direction.y +
            start_direction.z * start_direction.z));

    return generate_branch(length, rot_axis, angle, start_radius, end_radius, seg_num, curve_angle, exp);
}

