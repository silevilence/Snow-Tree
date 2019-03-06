//
// Created by lenovo on 2019/3/5.
//
/**!
 * @file MyTree.h
 * @brief 树类，用来生成树的网格
 *
 * @author wzx
 */

#ifndef TREE_MYTREE_H
#define TREE_MYTREE_H

#include <glm/vec3.hpp>
#include "Point.h"

class MyTree {
public:

private:
    /**!
     * 生成指定圆心，半径，点数的点
     * @param point 圆心点
     * @param precision 圆周上的点数
     * @return 生成的点的坐标，从0度开始逆时针排列
     */
    static glm::vec3* _Circle_Vertices(const Point& point, const int& precision);


};

#endif //TREE_MYTREE_H
