//
// Created by lenovo on 2019/3/14.
//

#ifndef TREE_SIMPLETREEBRANCH_H
#define TREE_SIMPLETREEBRANCH_H


#include <vector>
#include "Mesh.h"
#include "Point.h"

class SimpleTreeBranch {
public:
    std::vector<Point> points;
//    glm::vec3 position;
//    float rot_z;
//    float rot_y;
    unsigned int precision;

    SimpleTreeBranch(const std::vector<Point> &points, const glm::vec3 &position, float rot_z, float rot_y,
                     unsigned int precision = 20);

    SimpleTreeBranch(const Point *points, unsigned int p_num, const glm::vec3 &position, float rot_z, float rot_y,
                     unsigned int precision = 20);

    SimpleTreeBranch(const std::vector<Point> &points, const glm::mat4 &transform, unsigned int precision = 20);

    SimpleTreeBranch();

    SimpleTreeBranch(const SimpleTreeBranch &branch);

    SimpleTreeBranch(SimpleTreeBranch &&branch) noexcept;

    ~SimpleTreeBranch();

    SimpleTreeBranch &operator=(const SimpleTreeBranch &branch);

    SimpleTreeBranch &operator=(SimpleTreeBranch &&branch) noexcept;

    void generate_mesh(const bool &cal_normal = true);

    void draw(const glm::mat4 &transform, Shader shader);

    void update_points();

private:
    Mesh mesh;
    glm::mat4 _transform;

    void _copy(const SimpleTreeBranch &branch);

    void _move(SimpleTreeBranch &branch);
};


#endif //TREE_SIMPLETREEBRANCH_H
