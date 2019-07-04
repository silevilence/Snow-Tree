#include <utility>

//
// Created by lenovo on 2019/3/14.
//

#include "SimpleTree.h"
#include "MyTree.h"

SimpleTree::SimpleTree(std::vector<SimpleTreeBranch> branches) : branches(std::move(branches)) {
}

void SimpleTree::draw(const glm::mat4 &transform, Shader shader) {
    for(auto &branch: branches) {
        branch.draw(transform, shader);
    }
}

SimpleTree::SimpleTree(const SimpleTreeBranch *branches, const unsigned int &b_num) {
    this->branches = std::vector<SimpleTreeBranch>(branches, branches + b_num);
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-stack-address"

SimpleTree SimpleTree::generate_simple_tree() {
    using Branch = SimpleTreeBranch;

    std::vector<Branch> branches;

    auto points_main = MyTree::generate_branch(3, glm::vec3(0, 0, 1), 0, 1, 0, 50, 1, 2);
    branches.emplace_back(points_main, 51, glm::vec3(0), 0, 0, 50);
    const Branch &main = branches[0];
    delete points_main;

    // todo: 生成一堆树枝

    return SimpleTree(branches);
}

#pragma clang diagnostic pop

SimpleTree::SimpleTree(SimpleTree &&tree) noexcept {
    this->branches = std::move(tree.branches);
}

SimpleTree &SimpleTree::operator=(SimpleTree &&tree) noexcept {
    this->branches = std::move(tree.branches);

    return *this;
}

void SimpleTree::reset() {
    for(auto &branch : branches) {
        branch.reset(true);
    }
}

bool SimpleTree::complete_calculate() {
    bool stop = false;

    for(auto & branch : branches) {
        stop = branch.complete_calculate(true) or stop;
    }

    return stop;
}

