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
#include "Mesh.h"

class MyTree {
public:
    /**!
     * 根据顺序排列的点生成圆柱体网格
     * @param points 点集
     * @param pointNum 点的数量
     * @param precision 一个圆周的点的数量
     * @return 生成的圆柱网格
     */
    static Mesh _Create_Cylinders(const Point *points, int pointNum, int precision);

private:
    /**!
     * 生成指定圆心，半径，点数的点
     * @param point 圆心点
     * @param precision 圆周上的点数
     * @return 生成的点的坐标，从0度开始逆时针排列
     */
    static Vertex *_Circle_Vertices(const Point &point, const int &precision);

    /**!
     * 生成单段圆柱的三角形
     * @param precision 一个圆周的点的数量
     * @param bottom_start 底部圆的点的起始索引
     * @param top_start 顶部圆的点的起始索引
     * @return 生成的三角形数组
     */
    static int *_Create_Cylinder_Triangles(int precision, int bottom_start = -1, int top_start = -1);
};

#endif //TREE_MYTREE_H
