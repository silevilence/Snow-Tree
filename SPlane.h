//
// Created by lenovo on 2019/11/27.
//

#ifndef TREE_SPLANE_H
#define TREE_SPLANE_H

#include <vector>
#include "Vertex.h"
#include "Mesh.h"
#include "SimpleTreeBranch.h"

class SPlane {
public:
    std::vector<Vertex> vertices;

    std::vector<int> index1;
    std::vector<int> index2;
    std::vector<int> index3;

    Mesh *meshes;

    int PRECISION = 100;
    float rot_angle = 0.F;

    const SimpleTreeBranch *const branch = nullptr;
public:
    SPlane();

    SPlane(const SimpleTreeBranch &branch, const float &accumulate_angle = 30.F);

    void change_mode(int m);

    int get_mode();

    void draw(glm::mat4 transform, Shader shader);

private:
    int mode;

    void generate_points();

    void generate_index();

    void setup_mesh();

    static int *
    rectangular_index(const int &i1, const int &i2, const int &i3, const int &i4, const int &mode = 0);
};


#endif //TREE_SPLANE_H
