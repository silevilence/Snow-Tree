//
// Created by lenovo on 2019/8/17.
//

#ifndef TREE_SNOWPARTICLE_H
#define TREE_SNOWPARTICLE_H


#include "MyTree.h"

class SnowParticle : public MyTree {
public:
    float radius;
    int seg_num = 10;

    void generate_mesh();

    void draw(const glm::mat4 &transform, Shader shader);

private:
    Mesh mesh;
public:
    static Point *generate_particle_points(const float &radius, const int &seg_num = 10);
};


#endif //TREE_SNOWPARTICLE_H
