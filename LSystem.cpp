//
// Created by lenovo on 2019/3/18.
//

#include <time.h>
#include <random>
#include <stack>

#include "glm/gtc/matrix_transform.hpp"

#include "LSystem.h"
#include "MyTree.h"

using std::string;

string LSystem::_param_l_rule(string str, int index) {
    /*产生式
    A(0)
    A(d):d > 0 -> A(d-1)
    A(d):d = 0 -> F(1)[&A(0)][*A(0)][/A(0)]F(1)A(0)
    F(a):* -> F(a * 2)
    */
    auto s_rand_seed = (unsigned int) time(nullptr);
    std::default_random_engine e(s_rand_seed);
    std::uniform_int_distribution<int> random(0, 1);
    switch(str[index]) {
        case 'A': {
            int i;
            for(i = index + 2; str[i] != ')'; i++);//i 表示 ')'的索引
            string str_d(str, static_cast<unsigned int>(index + 2), static_cast<unsigned int>(i - index - 2));
            int d = stoi(str_d);
            if(d > 0) {
                d--;
                char pD[2];
//                _itoa_s(d, pD, 2, 10);
                sprintf(pD, "%d", d);
                string strCurd(pD);
                string strA = "A(" + strCurd + ")";
                return strA;
            } else {
                if(d == 0) {
//                    srand(s_rand_seed++);
                    int rdm = random(e);
                    switch(rdm) {
                        case 0: {
                            //return "F(1)[&A(0)][*A(0)][/A(0)]F(1)A(0)";
                            return "F(1)[&A(0)][*A(0)][/A(0)]A(0)";
                        }
                        case 1: {
                            //return "F(1)[&A(0)][*A(0)][/A(1)]F(1)A(0)";
                            return "F(1)[&A(0)][*A(0)][/A(1)]A(0)";
                        }
                        default: {
                            return "";
                        }
                    }
                }
            }
        }
        case 'F': {
            int i;
            for(i = index + 2; str[i] != ')'; i++);
            string str_a(str, static_cast<unsigned int>(index + 2), static_cast<unsigned int>(i - index - 2));
            int a = stoi(str_a);
            a = a * 2;
            char pA[4];
//            _itoa_s(a, pA, 4, 10);
            sprintf(pA, "%d", a);
            string strCura(pA);
            string strF = "F(" + strCura + ")";
            return strF;
        }
        case '[': {
            return "[";
        }
        case ']': {
            return "]";
        }
        case '&': {
            return "&";
        }
        case '*': {
            return "*";
        }
        case '/': {
            return "/";
        }
        default: {
            return "";
        }
    }
}

std::string LSystem::param_iterator(std::string str, int times) {
    string strTemp;
    for(int i = 0; i < times; i++) {
        for(int j = 0; j < str.length(); j++) {
            strTemp += _param_l_rule(str, j);
        }
        str = strTemp;
        strTemp = "";
    }
    return str;
}

SimpleTree LSystem::param_l_interpret(std::string str) {
    glm::mat4 transform(1);
    std::stack<glm::mat4> trans_stack;
    std::vector<SimpleTreeBranch> branches;
//    SimpleTreeBranch *current;
//    std::stack<SimpleTreeBranch *> branch_stack;

    auto glPushMatrix = [&]()mutable {
        trans_stack.push(transform);
//        branch_stack.push(current);
    };
    auto glPopMatrix = [&]()mutable {
        transform = trans_stack.top();
        trans_stack.pop();
//        current = branch_stack.top();
//        branch_stack.pop();
    };
    auto glRotatef = [&](float angle, float x, float y, float z)mutable {
        transform = glm::rotate(transform, glm::radians(angle), glm::vec3(x, y, z));
    };
    auto glScalef = [&](float x, float y, float z)mutable {
        transform = glm::scale(transform, glm::vec3(x, y, z));
    };
    auto glTranslatef = [&](float x, float y, float z)mutable {
        transform = glm::translate(transform, glm::vec3(x, y, z));
    };
    auto Frustum = [&](float bottom, float top, float height)mutable {
        std::vector<Point> points;
        if(1 >= height) {
            points = {Point(glm::vec3(0), bottom, glm::vec3(0), 0),
                      Point(glm::vec3(0, height, 0), top, glm::vec3(0), 0)};
        } else {
            auto bps = MyTree::generate_branch(height, glm::vec3(0), 0, bottom, top, (int) height, 0, 1);
            points = std::vector<Point>(bps, bps + (int) height + 1);
            delete bps;
        }
//        if(not branch_stack.empty()) {
//            SimpleTreeBranch branch(points, transform, 20);
////            current->parent = &branch;
//            branch_stack.top()->add_child(branch);
//        } else {
//            branches.emplace_back(points, transform, 20);
//            current = &branches.back();
//        }
        branches.emplace_back(points, transform, 20);
    };
    auto bezierLeafDisplay = [&]()mutable {
        // todo: draw leaf
    };

    for(int i = 0; i < str.length(); i++) {
        switch(str[i]) {
            case 'F': {
                int j;
                for(j = i + 2; str[j] != ')'; j++);
                string str_a(str, static_cast<unsigned int>(i + 2), static_cast<unsigned int>(j - i - 2));
                int a = stoi(str_a);
//                glBindTexture(GL_TEXTURE_2D, texName[0]);
//                glEnable(GL_TEXTURE_2D);
                //auxSolidCylinder(0.05, branchLength);
                //Cylinder(0.05*a, a);
                Frustum(static_cast<float>(0.02 * a), static_cast<float>(0.02 * a), a);
                glDisable(GL_TEXTURE_2D);
                glTranslatef(0, a, 0);
                if(a == 1)//有叶子
                {
                    //glRotatef(120, 0, 1, 0);
                    glPushMatrix();
                    /*glRotatef(45, 1, 0, 0);
                    glBegin(GL_LINES);
                    glVertex3f(0, 0, 0);
                    glVertex3f(0, 1, 0);
                    glEnd();
                    glTranslatef(0, 1, 0);
                    glBindTexture(GL_TEXTURE_2D, texName[1]);
                    glEnable(GL_TEXTURE_2D);
                    glBegin(GL_QUADS);
                    glTexCoord2f(1.0, 1.0);		glVertex3f( 0.5,  0.5, 0.0);
                    glTexCoord2f(0.0, 1.0);		glVertex3f(-0.5,  0.5, 0.0);
                    glTexCoord2f(0.0, 0.0);		glVertex3f(-0.5, -0.5, 0.0);
                    glTexCoord2f(1.0, 0.0);		glVertex3f( 0.5, -0.5, 0.0);
                    glEnd();
                    glDisable(GL_TEXTURE_2D);*/
                    glScalef(0.1, 0.1, 0.1);
                    glRotatef(-30, 1, 0, 0);
                    glTranslatef(0, 9, -1.5f);
//                    glBindTexture(GL_TEXTURE_2D, texName[1]);
                    bezierLeafDisplay();//绘制叶片
                    glPopMatrix();
                }
                break;
            }
            case '[': {
                glPushMatrix();//入栈
                break;
            }
            case ']': {
                glPopMatrix();//出栈
                break;
            }
            case '&': {
                glRotatef(0, 0, 1, 0);
                glRotatef(45, 1, 0, 0);
                break;
            }
            case '*': {
                glRotatef(120, 0, 1, 0);
                glRotatef(45, 1, 0, 0);
                break;
            }
            case '/': {
                glRotatef(-120, 0, 1, 0);
                glRotatef(45, 1, 0, 0);
                break;
            }
            default:
                break;
        }
    }

    return SimpleTree(branches);
}
