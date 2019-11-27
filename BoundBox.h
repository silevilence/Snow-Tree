//
// Created by lenovo on 2019/8/9.
//

#ifndef TREE_BOUNDBOX_H
#define TREE_BOUNDBOX_H


class SimpleTreeBranch;

class BoundBox {
public:
    SimpleTreeBranch *branch;
    float xmin;
    float xmax;
    float ymin;
    float ymax;
    float zmin;
    float zmax;
};


#endif //TREE_BOUNDBOX_H
