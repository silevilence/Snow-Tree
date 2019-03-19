//
// Created by lenovo on 2019/3/18.
//

#ifndef TREE_LSYSTEM_H
#define TREE_LSYSTEM_H


#include <string>
#include "SimpleTree.h"

class LSystem {
public:
    static std::string param_iterator(std::string str, int times);

    static SimpleTree param_l_interpret(std::string str);

private:
    static std::string _param_l_rule(std::string str, int index);
};


#endif //TREE_LSYSTEM_H
