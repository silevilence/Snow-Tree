//
// Created by lenovo on 2019/12/7.
//

#ifndef TREE_PERLIN_H
#define TREE_PERLIN_H


class Perlin {
private:
    static int *permutation;
    static int *p;           // Doubled permutation to avoid overflow

public:
    Perlin(int repeat = -1);

    int repeat;

    double perlin(double x, double y, double z);

    double OctavePerlin(double x, double y, double z, int octaves, double persistence);

    int inc(int num);

    static double grad(int hash, double x, double y, double z);


    static double fade(double t);

    static double lerp(double a, double b, double x);
};


#endif //TREE_PERLIN_H
