//
// Created by lenovo on 2019/3/14.
//

#ifndef TREE_SIMPLETREEBRANCH_H
#define TREE_SIMPLETREEBRANCH_H


#include <vector>
#include <memory>
#include "Mesh.h"
#include "Point.h"

class BoundBox;

class SimpleTreeBranch {
public:
    std::vector<Point> points;
//    glm::vec3 position;
//    float rot_z;
//    float rot_y;
    unsigned int precision{};

    BoundBox *box;
    BoundBox *tree_box;

    float b_theta{};
    float length{};

    SimpleTreeBranch *parent{};
    std::vector<SimpleTreeBranch *> children;

    SimpleTreeBranch(std::vector<Point> points, const glm::vec3 &position, float rot_z, float rot_y,
                     unsigned int precision = 20, float length = 2);

    SimpleTreeBranch(const Point *points, unsigned int p_num, const glm::vec3 &position, float rot_z,
                     float rot_y,
                     unsigned int precision = 20, float length = 2);

    SimpleTreeBranch(std::vector<Point> points, const glm::mat4 &transform, unsigned int precision = 20,
                     float b_theta = 0, float length = 2);

    SimpleTreeBranch();

    SimpleTreeBranch(const SimpleTreeBranch &branch);

    SimpleTreeBranch(SimpleTreeBranch &&branch) noexcept;

    ~SimpleTreeBranch();

    SimpleTreeBranch &operator=(const SimpleTreeBranch &branch);

    SimpleTreeBranch &operator=(SimpleTreeBranch &&branch) noexcept;

    void generate_mesh(const bool &cal_normal = true);

    void draw(const glm::mat4 &transform, Shader shader);

    void update_points();

    bool uniform_load_pressure(const float &q);

    bool uniform_load_pressure(const float &q, float q_theta);

    bool concentrated_load_pressure(const float &F, float f_theta);

    void add_concentrated_force(const float &F, float f_theta);

    void reset(const bool &recursive = true);

    bool complete_calculate(const bool &recursive = true);

    void add_child(SimpleTreeBranch &branch);

    BoundBox *create_bound_box();

    BoundBox *create_bound_box_tree();

private:
    Mesh mesh;

    glm::mat4 _transform;

    glm::vec3 force_sum;

    bool changed;

    void _copy(const SimpleTreeBranch &branch);

    void _move(SimpleTreeBranch &branch);
};


#endif //TREE_SIMPLETREEBRANCH_H
