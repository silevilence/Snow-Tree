//
// Created by lenovo on 2019/3/14.
//

#ifndef TREE_SIMPLETREE_H
#define TREE_SIMPLETREE_H


#include <vector>
#include "SimpleTreeBranch.h"
#include "Point.h"

class SimpleTree {
public:
    explicit SimpleTree(const std::vector<SimpleTreeBranch> &branches);

    SimpleTree(const SimpleTreeBranch *branches, const unsigned int &b_num);

    SimpleTree(const SimpleTree &tree) = default;

    SimpleTree(SimpleTree &&tree) noexcept;

    SimpleTree &operator=(const SimpleTree &tree) = default;

    SimpleTree &operator=(SimpleTree &&tree) noexcept;

    SimpleTree() = delete;

    void draw(const glm::mat4 &transform, Shader shader);

    static SimpleTree generate_simple_tree();

private:
public:
    std::vector<SimpleTreeBranch> branches;
};


#endif //TREE_SIMPLETREE_H
