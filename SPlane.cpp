//
// Created by lenovo on 2019/11/27.
//

#include "SPlane.h"
#include "Perlin.h"

#include <glm/gtc/matrix_transform.hpp>

#include <random>
#include <ctime>
#include <iostream>

//const int PRECISION = 100;
const float PLANE_SIZE = 1;

/**
 * 根据首尾点和数量插值顶点
 * @param vertex1
 * @param vertex2
 * @param num
 * @return
 */
std::vector<Vertex> point_lerp(const Vertex &vertex1, const Vertex &vertex2, const int &num);

/**
 * 返回缩放因子
 * @param x [0,1]
 * @param y [0,1]
 * @return E^(-\[Pi] ((-1+2 x)^2+(-1+2 y)^2))
 */
inline float magnification(const float &x, const float &y);

SPlane::SPlane()
        : mode(1), branch(nullptr) {
    generate_points();
    generate_index();
    setup_mesh();
}

void SPlane::generate_points() {
    vertices.clear();

    std::default_random_engine e(time(nullptr));
//    std::uniform_real_distribution<float> u(0, PLANE_SIZE / 10);
    std::uniform_real_distribution<float> u(0, 0.01);
    auto *perlin = new Perlin;

    for(int i = 0; i < PRECISION; ++i) {
        for(int j = 0; j < PRECISION; ++j) {
            float x = j * PLANE_SIZE / PRECISION;
            float y = 0;
            float z = i * PLANE_SIZE / PRECISION;
            y = (float) perlin->OctavePerlin(x * 2, y, z * 2, 5, 2) * .4F
                * magnification((float) i / PRECISION, (float) j / PRECISION);
//            if(not(i == 0 or i == PRECISION - 1 or j == 0 or j == PRECISION - 1)) {
////                y = u(e);
////                y = 0.1 * j * PLANE_SIZE / PRECISION + 0.005 - u(e);
////                y = (float) perlin->perlin(x * 2, y * 2, z * 2);
//                y = (float) perlin->OctavePerlin(x * 2, y, z * 2, 5, 2) * .1F;
//            }

            Vertex p;
            p.position = glm::vec3(x, y, z);

            vertices.push_back(p);
        }
    }

    delete perlin;
}

void SPlane::generate_index() {
    // 1. 全部正向
    index1.clear();
    for(int i = 0; i < vertices.size() - PRECISION; ++i) {
        if((i + 1) % PRECISION == 0) {
            continue;
        }
        auto index = rectangular_index(i, i + 1, i + PRECISION, i + PRECISION + 1, 0);
        index1.insert(index1.end(), index, index + 6);
        delete index;
    }

    // 2. 正反交替
    index2.clear();
    for(int i = 0; i < vertices.size() - PRECISION; ++i) {
        if((i + 1) % PRECISION == 0) {
            continue;
        }
        auto index = rectangular_index(i, i + 1, i + PRECISION, i + PRECISION + 1, (i % PRECISION) % 2);
        index2.insert(index2.end(), index, index + 6);
        delete index;
    }

    // 3. 完全随机
    index3.clear();

    std::default_random_engine e(time(nullptr));
    std::uniform_int_distribution<int> u(0, 1);

    for(int i = 0; i < vertices.size() - PRECISION; ++i) {
        if((i + 1) % PRECISION == 0) {
            continue;
        }
        auto index = rectangular_index(i, i + 1, i + PRECISION, i + PRECISION + 1, u(e));
        index3.insert(index3.end(), index, index + 6);
        delete index;
    }
}

int *SPlane::rectangular_index(const int &i1, const int &i2, const int &i3, const int &i4, const int &mode) {
    if(mode == 0) {
        return new int[6]{i1, i3, i2, i2, i3, i4};
    } else {
        return new int[6]{i1, i3, i4, i1, i4, i2};
    }
}

void SPlane::setup_mesh() {
    meshes = new Mesh[3]{};

    // mesh 1
    meshes[0].vertices = &vertices[0];
    meshes[0].vertices_num = vertices.size();
    meshes[0].indices = &index1[0];
    meshes[0].indices_num = index1.size();
    meshes[0].setup_mesh();

    // mesh 2
    meshes[1].vertices = &vertices[0];
    meshes[1].vertices_num = vertices.size();
    meshes[1].indices = &index2[0];
    meshes[1].indices_num = index2.size();
    meshes[1].setup_mesh();

    // mesh 3
    meshes[2].vertices = &vertices[0];
    meshes[2].vertices_num = vertices.size();
    meshes[2].indices = &index3[0];
    meshes[2].indices_num = index3.size();
    meshes[2].setup_mesh();
}

void SPlane::change_mode(int m) {
    this->mode = m < 1 ? 1 : (m > 3 ? 3 : m);
}

int SPlane::get_mode() {
    return mode;
}

void SPlane::draw(glm::mat4 transform, Shader shader) {
    shader.use();

//    shader.set_matrix4("model", transform);
    if(branch == nullptr) {
        shader.set_matrix4("model", transform);
    } else {
        glm::mat4 rot = glm::rotate(glm::mat4(1), glm::radians(-rot_angle), glm::vec3(0, 0, -1));
//        glm::mat4 rot = glm::rotate(glm::mat4(1), glm::radians(-90.F), glm::vec3(0, 0, -1));
//        shader.set_matrix4("model", transform * *branch->last_transform * rot);
        shader.set_matrix4("model", *branch->last_transform * rot);
    }

    meshes[mode - 1].draw(shader);
}

SPlane::SPlane(const SimpleTreeBranch &branch, const float &accumulate_angle, const float &height)
        : mode(1), branch(&branch) {
    const auto &mesh = branch.mesh;
    auto rot_z = branch.rot_z;
//    auto rot_y = branch.rot_y;

    // -180~180
    while(rot_z > 180)
        rot_z -= 360;
    while(rot_z < -180)
        rot_z += 360;

    auto step = 360.F / branch.precision;

    // 点的索引范围，需要分情况处理
    int mini, maxi;
    // 左倾，此时min实际上比max要大
    // 用取模操作防止溢出
    if(rot_z < 0) {
        /*
         *     *
         *   *  *
         * *     *
         *  *  X
         *   *
         */
        mini = (int) ceilf((360 - accumulate_angle) / step);
        /*
         *     *
         *   *  X
         * *     *
         *  *  *
         *   *
         */
        // 由于按照惯例左闭右开，需要再+1，即放弃floor使用ceil
        // 为防止出现整除，加上一个小值
        maxi = (int) ceilf(accumulate_angle / step + 0.0001F);

        PRECISION = (int) branch.precision - mini + maxi;
    } else { // 右倾
        /*
         *     *
         *   X  *
         * *     *
         *  *  *
         *   *
         */
        mini = (int) ceilf((180 - accumulate_angle) / step);
        /*
         *     *
         *   *  *
         * *     *
         *  X  *
         *   *
         */
        // 操作同上
        maxi = (int) ceilf((180 + accumulate_angle) / step + 0.0001F);

        PRECISION = maxi - mini;
    }

    // 当精度过低进需要做插值处理
    int prec = PRECISION;
    if(PRECISION < 10) {
        PRECISION += 10 * (prec - 1);
    }

    auto *perlin = new Perlin;
    // 旋转角计算
//    rot_angle = rot_z > 0 ? 90 - rot_z : -(90 + rot_z);
    rot_angle = rot_z > 0 ? 90 : -90;
    glm::mat4 rot = glm::rotate(glm::mat4(1), glm::radians(rot_angle), glm::vec3(0, 0, -1));
    // 点
    // X--*--*--*
    for(int i = 0; i < branch.points.size(); ++i) {
        /*
         *     *
         *   *  *
         * *     X
         *  *  *
         *   *
         */
        for(int j = 0; j < prec; ++j) {
            const Vertex &vertex = mesh.vertices[i * branch.precision + (j + mini) % branch.precision];
            glm::vec4 pos4 = rot * glm::vec4(vertex.position, 1);
            Vertex v;
            v.position = glm::vec3(pos4.x, pos4.y + 0.001F, pos4.z);

            // 插值
            if(j != 0 and prec != PRECISION) {
                auto lerped_vertices = point_lerp(this->vertices.back(), v, 10);
                this->vertices.insert(this->vertices.end(), lerped_vertices.begin(), lerped_vertices.end());
            }

            this->vertices.push_back(v);
        }
    }

    // perlin
    // 当为边缘点时形成边缘随机，当为中间点时作为高度
//    for(int i = PRECISION; i < this->vertices.size() - PRECISION; ++i) {
    for(int i = 0; i < this->vertices.size(); ++i) {
        auto &pos = this->vertices[i].position;
        auto distance = perlin->OctavePerlin(fabsf(pos.x), fabsf(pos.y), fabsf(pos.z), 5, 2);
//        std::cout<<distance<<std::endl;

        auto trans = glm::mat4(1);
        glm::vec3 prev;

        auto roted = rot * glm::vec4(branch.points[i / PRECISION].position, 1);
        prev = glm::vec3(roted.x, roted.y, roted.z);

        trans = glm::translate(trans,
                               glm::normalize(pos - prev) *
                               (float) ((height + distance * .03F) *
                                        magnification(float(i / PRECISION) / branch.points.size(),
                                                      float(i % PRECISION) / PRECISION)));

        /*
         * ***
         * ***
         * X**
         */
        if(i % PRECISION == 0) {
            prev = this->vertices[i + 1].position;
        }
            /*
             * ***
             * ***
             * **X
             */
        else if((i + 1) % PRECISION == 0) {
            prev = this->vertices[i - 1].position;
        }
            /*
             * ***    *X*
             * *** or ***
             * *X*    ***
             */
            // 不需要
//        else if(i / PRECISION == 0 or i / PRECISION == branch.points.size() - 1) {
//            continue;
//        }
            /*
             * ***
             * *X*
             * ***
             */
        else {
            prev = glm::vec3(0);
        }

        prev = glm::vec3(0);
        if(prev != glm::vec3(0)) {
            trans = glm::translate(trans, glm::normalize(pos - prev) *
                                          (float) ((height + distance * .02F) *
                                                   magnification(float(i / PRECISION) / branch.points.size(),
                                                                 0.5F)));
        }

        auto moved_pos = trans * glm::vec4(pos, 1);
        pos.x = moved_pos.x;
        pos.y = moved_pos.y;
        pos.z = moved_pos.z;
    }

    // index
    generate_index();
//    index1.clear();
//    index1.insert(index1.end(), mesh.indices, mesh.indices + mesh.indices_num);
//    index2 = index3 = index1;

    setup_mesh();

    delete perlin;
}

std::vector<Vertex> point_lerp(const Vertex &vertex1, const Vertex &vertex2, const int &num) {
    std::vector<Vertex> result;

    auto nn = (float) num + 1;
    for(int i = 1; i <= num; ++i) {
        auto ii = (float) i;
        Vertex v = {
                .position = vertex1.position + (vertex2.position - vertex1.position) * ii / nn,
                .normal = vertex1.normal + (vertex2.normal - vertex1.normal) * ii / nn,
                .tex_coord = vertex1.tex_coord + (vertex2.tex_coord - vertex1.tex_coord) * ii / nn
        };
//        v.position = vertex1.position + (vertex2.position - vertex1.position) * ii / nn;
//        v.normal = vertex1.normal + step_norm * ii;
//        v.tex_coord = vertex1.tex_coord + step_tex * ii;

        result.push_back(v);
    }

    return result;
}

float magnification(const float &x, const float &y) {
    // E^(-\[Pi] ((-1+2 x)^2+(-1+2 y)^2))
//    const static float ep = 0.0432139F; // e^(-pi)
//    float xx = x + x - 1; // 2x-1
//    float yy = y + y - 1; // 2y-1
//
//    return powf(ep, xx * xx + yy * yy);

    // 1/4 (1+Cos[\[Pi] (-1+2 x)]) (1+Cos[\[Pi] (-1+2 y)]) = Sin[\[Pi] x]^2 Sin[\[Pi] y]^2
    const static float pi = 3.141592654F; // pi
    float spx = sinf(pi * x);
    float spy = sinf(pi * y);
    return spx * spx * spy * spy;

    // Sqrt[(1-(-1+2 x)^2) (1-(-1+2 y)^2)]
//    float xx = x + x - 1; // 2x-1
//    float yy = y + y - 1; // 2y-1
//    return sqrtf((1 - xx * xx) * (1 - yy * yy));
}
