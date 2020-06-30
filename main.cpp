#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include <time.h>
#include <stb_image.h>

#include "ResourceManager.h"
#include "MyTree.h"
#include "Camera.h"

#include "LSystem.h"
#include "Snow.h"
#include "SPlane.h"
#include "ParticleGenerator.h"

const GLuint SCREEN_WIDTH = 800;
const GLuint SCREEN_HEIGHT = 600;
GLuint fps_limit = 0;

auto delta_time = 0.0f;

float last_x, last_y;
const float sensitivity = 0.3f;
// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f)); // NOLINT(cert-err58-cpp)

SPlane *snow_plane;

glm::vec3 force = glm::vec3(0.0f);
const float FORCE_CLEAR_INTERVAL = 0.2f;
double last_change_time;

particle_generator *pg;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

unsigned int loadCubemap(std::vector<std::string> faces);

float cal_omega(const float &E, const float &I, const float &q, const float &L, const float &x);

float cal_theta(const float &E, const float &I, const float &q, const float &L, const float &x);

int main(int argc, char *argv[]) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tree", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    gladLoadGLLoader(GLADloadproc(glfwGetProcAddress));
    glGetError(); // Call it once to catch glewInit() bug, all other errors are now from our application.

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // OpenGL configuration
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    // 不知道为什么要声明顺时针才能让逆时针的点正常渲染
    glFrontFace(GL_CW);
//    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float skyboxVertices[] = {
            // positions
            -1.0F, 1.0F, -1.0F,
            -1.0F, -1.0F, -1.0F,
            1.0F, -1.0F, -1.0F,
            1.0F, -1.0F, -1.0F,
            1.0F, 1.0F, -1.0F,
            -1.0F, 1.0F, -1.0F,

            -1.0F, -1.0F, 1.0F,
            -1.0F, -1.0F, -1.0F,
            -1.0F, 1.0F, -1.0F,
            -1.0F, 1.0F, -1.0F,
            -1.0F, 1.0F, 1.0F,
            -1.0F, -1.0F, 1.0F,

            1.0F, -1.0F, -1.0F,
            1.0F, -1.0F, 1.0F,
            1.0F, 1.0F, 1.0F,
            1.0F, 1.0F, 1.0F,
            1.0F, 1.0F, -1.0F,
            1.0F, -1.0F, -1.0F,

            -1.0F, -1.0F, 1.0F,
            -1.0F, 1.0F, 1.0F,
            1.0F, 1.0F, 1.0F,
            1.0F, 1.0F, 1.0F,
            1.0F, -1.0F, 1.0F,
            -1.0F, -1.0F, 1.0F,

            -1.0F, 1.0F, -1.0F,
            1.0F, 1.0F, -1.0F,
            1.0F, 1.0F, 1.0F,
            1.0F, 1.0F, 1.0F,
            -1.0F, 1.0F, 1.0F,
            -1.0F, 1.0F, -1.0F,

            -1.0F, -1.0F, -1.0F,
            -1.0F, -1.0F, 1.0F,
            1.0F, -1.0F, -1.0F,
            1.0F, -1.0F, -1.0F,
            -1.0F, -1.0F, 1.0F,
            1.0F, -1.0F, 1.0F
    };

    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) nullptr);

    Shader shader;
    shader.compile_from_file("shaders/object.vert", "shaders/object.frag");
    Shader skyboxShader;
    skyboxShader.compile_from_file("shaders/skybox.vert", "shaders/skybox.frag");

    // load textures
    std::vector<std::string> faces{
            "textures/right2.png",
            "textures/left2.png",
            "textures/top2.png",
            "textures/bottom2.png",
            "textures/front2.png",
            "textures/back2.png"
    };
    unsigned int cubemapTexture = loadCubemap(faces);
    skyboxShader.use();
    skyboxShader.set_integer("skybox", 0);

    //resource_manager::load_shader("shaders/particle.vert", "shaders/particle.frag", nullptr, "Particle");
    //resource_manager::load_texture("textures/snow_near.png", true, "Particle");
    //const auto particle_shader = resource_manager::get_shader("Particle");
    //const auto particle_texture = resource_manager::get_texture("Particle");

    auto frame_render = 0;
    auto frame_update = 0;
    auto frame_timer = 0.0f;

    auto last_frame = 0.0f;

    last_x = SCREEN_WIDTH / 2.f;
    last_y = SCREEN_HEIGHT / 2.f;

//    Point points[] = {Point(glm::vec3(0, 0, 0), 1.1, glm::vec3(1), 0),
//                      Point(glm::vec3(0, 0.1, 0), 1, glm::vec3(0), 0),
//                      Point(glm::vec3(0, 0.9, 0), 1, glm::vec3(0), 0),
//                      Point(glm::vec3(0, 1, 0), 0.9, glm::vec3(1), 0)};
//    Mesh tree = MyTree::Create_Cylinders(points, 4, 20);
//    tree.setup_mesh();

//    Point points[] = {Point(glm::vec3(0, 0, 0), 1, glm::vec3(1,1,0), 25),
//                      Point(glm::vec3(0, 0, 0), 0, glm::vec3(0), 0)};
//    Mesh tree = MyTree::Create_Cylinders(points, 2, 20);

//    int times = 72;
//    auto points = MyTree::generate_circular_helix(1, 10, 0.5, 0.1, times);
//    Mesh tree = MyTree::Create_Cylinders(points, times, 20);

//    int seg_num = 10;
//    auto points = MyTree::generate_branch(2, glm::vec3(0, 0, -1), 1, 0.5, 0, seg_num, 1, 2);
//    Mesh &&treee = MyTree::Create_Cylinders(points, seg_num + 1, 50);
//
//    treee.setup_mesh(true);
//    delete points;
//
//    const int branch_num = 5;
//    Mesh branches[branch_num];
//    auto ps_branch = MyTree::generate_branch(1.5f, glm::vec3(0, 0, 1), 10, 0.1, 0, seg_num, 2, 2);
//    for(auto &branch : branches) {
//        branch = std::move(MyTree::Create_Cylinders(ps_branch, seg_num + 1, 20));
//
//        branch.setup_mesh(true);
//    }
//    Mesh &&branch = MyTree::Create_Cylinders(ps_branch, seg_num + 1, 20);
//
////    branch.setup_mesh();
//
//    delete ps_branch;

//    auto tree_str = LSystem::param_iterator("A(0)", 5);
//    std::cout << tree_str << std::endl;
//    auto l_tree = LSystem::param_l_interpret(tree_str);

    const int SEG = 25;
    const float length = 1;

    std::vector<SimpleTreeBranch *> branches_vec;

    // root branch
    auto ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.1, 0.09, SEG, 0, 1, 8.77e9,
                                             1.f, 1.f, 2.f);
//    auto branch = MyTree::Create_Cylinders(ps_branch, SEG + 1, 20);
//    branch.setup_mesh();
    std::vector<Point> points(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    std::vector<SimpleTreeBranch> branches;
    float b_theta = 90.f;
    branches.emplace_back(points, glm::vec3(0), 0, 0, 20, length);
//    SimpleTreeBranch branch(points, glm::vec3(0), 90, 0);
    SimpleTree tree(branches);
    tree.branches[0].update_points();
    auto &branch1 = tree.branches[0];

    // branch2
    ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.08, 0.07, SEG, 0, 1, 8.77e9, 1.f,
                                        1.f, 2.f);
    std::vector<Point> points_ch2(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch branch2(points_ch2, glm::vec3(0), 40, 0, 20, length);
    branch2.update_points();
    branch1.add_child(branch2);
    branches_vec.push_back(&branch2);

    // branch3
    ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.07, 0.05, SEG, 0, 1, 8.77e9, 1.f,
                                        1.f, 2.f);
    std::vector<Point> points_ch3(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch branch3(points_ch3, glm::vec3(0), 20, 0, 20, length);
    branch3.update_points();
    branch2.add_child(branch3);
    branches_vec.push_back(&branch3);

    // branch4
    ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.05, 0.04, SEG, 0, 1, 8.77e9, 1.f,
                                        1.f, 2.f);
    std::vector<Point> points_ch4(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch branch4(points_ch4, glm::vec3(0), 10, 0, 20, length);
    branch4.update_points();
    branch3.add_child(branch4);
    branches_vec.push_back(&branch4);

    // branch5
    ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.04, 0., SEG, 0, 1, 8.77e9, 1.f,
                                        1.f, 2.f);
    std::vector<Point> points_ch5(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch branch5(points_ch5, glm::vec3(0), 20, 0, 20, length);
    branch5.update_points();
    branch4.add_child(branch5);
    branches_vec.push_back(&branch5);

    //--------------------------------------

    // branch6
    ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.04, 0., SEG, 0, 1, 8.77e9, 1.f,
                                        1.f, 2.f);
    std::vector<Point> points_ch6(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch branch6(points_ch6, glm::vec3(0), -10, 0, 20, length);
    branch6.update_points();
    branch4.add_child(branch6);
    branches_vec.push_back(&branch6);

    // branch7
    ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.07, 0.05, SEG, 0, 1, 8.77e9, 1.f,
                                        1.f, 2.f);
    std::vector<Point> points_ch7(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch branch7(points_ch7, glm::vec3(0), -10, 0, 20, length);
    branch7.update_points();
    branch2.add_child(branch7);
    branches_vec.push_back(&branch7);

    // branch8
    ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.05, 0., SEG, 0, 1, 8.77e9, 1.f,
                                        1.f, 2.f);
    std::vector<Point> points_ch8(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch branch8(points_ch8, glm::vec3(0), 10, 0, 20, length);
    branch8.update_points();
    branch7.add_child(branch8);
    branches_vec.push_back(&branch8);

    // branch9
    ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.05, 0., SEG, 0, 1, 8.77e9, 1.f,
                                        1.f, 2.f);
    std::vector<Point> points_ch9(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch branch9(points_ch9, glm::vec3(0), -20, 0, 20, length);
    branch9.update_points();
    branch7.add_child(branch9);
    branches_vec.push_back(&branch9);

    //------------------------------------

    // branch10
    ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.08, 0.07, SEG, 0, 1, 8.77e9, 1.f,
                                        1.f, 2.f);
    std::vector<Point> points_ch10(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch branch10(points_ch10, glm::vec3(0), 40, 40, 20, length);
    branch10.update_points();
    branch1.add_child(branch10);
    branches_vec.push_back(&branch10);

    // branch11
    ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.07, 0.06, SEG, 0, 1, 8.77e9, 1.f,
                                        1.f, 2.f);
    std::vector<Point> points_ch11(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch branch11(points_ch11, glm::vec3(0), 5, 0, 20, length);
    branch11.update_points();
    branch10.add_child(branch11);
    branches_vec.push_back(&branch11);

    // branch12
    ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.06, 0.05, SEG, 0, 1, 8.77e9, 1.f,
                                        1.f, 2.f);
    std::vector<Point> points_ch12(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch branch12(points_ch12, glm::vec3(0), 5, 0, 20, length);
    branch12.update_points();
    branch11.add_child(branch12);
    branches_vec.push_back(&branch12);

    // branch13
    ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.05, 0., SEG, 0, 1, 8.77e9, 1.f,
                                        1.f, 2.f);
    std::vector<Point> points_ch13(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch branch13(points_ch13, glm::vec3(0), 5, 0, 20, length);
    branch13.update_points();
    branch12.add_child(branch13);
    branches_vec.push_back(&branch13);

    // branch14
    ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.05, 0., SEG, 0, 1, 8.77e9, 1.f,
                                        1.f, 2.f);
    std::vector<Point> points_ch14(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch branch14(points_ch14, glm::vec3(0), -20, 0, 20, length);
    branch14.update_points();
    branch12.add_child(branch14);
    branches_vec.push_back(&branch14);

    //-----------------------

    // branch15
    ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.06, 0.05, SEG, 0, 1, 8.77e9, 1.f,
                                        1.f, 2.f);
    std::vector<Point> points_ch15(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch branch15(points_ch15, glm::vec3(0), -20, 0, 20, length);
    branch15.update_points();
    branch11.add_child(branch15);
    branches_vec.push_back(&branch15);

    // branch16
    ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.05, 0., SEG, 0, 1, 8.77e9, 1.f,
                                        1.f, 2.f);
    std::vector<Point> points_ch16(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch branch16(points_ch16, glm::vec3(0), 10, 0, 20, length);
    branch16.update_points();
    branch15.add_child(branch16);
    branches_vec.push_back(&branch16);

    // branch17
    ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.05, 0., SEG, 0, 1, 8.77e9, 1.f,
                                        1.f, 2.f);
    std::vector<Point> points_ch17(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch branch17(points_ch17, glm::vec3(0), -20, 0, 20, length);
    branch17.update_points();
    branch15.add_child(branch17);
    branches_vec.push_back(&branch17);

    // --------------------------

    // branch18
    ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.09, 0.07, SEG, 0, 1, 8.77e9, 1.f,
                                        1.f, 2.f);
    std::vector<Point> points_ch18(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch branch18(points_ch18, glm::vec3(0), 40, 130, 20, length);
    branch18.update_points();
    branch1.add_child(branch18);
    branches_vec.push_back(&branch18);

    // branch19
    ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.07, 0.05, SEG, 0, 1, 8.77e9, 1.f,
                                        1.f, 2.f);
    std::vector<Point> points_ch19(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch branch19(points_ch19, glm::vec3(0), 20, 0, 20, length);
    branch19.update_points();
    branch18.add_child(branch19);
    branches_vec.push_back(&branch19);

    // branch20
    ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.05, 0., SEG, 0, 1, 8.77e9, 1.f,
                                        1.f, 2.f);
    std::vector<Point> points_ch20(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch branch20(points_ch20, glm::vec3(0), 30, 0, 20, length);
    branch20.update_points();
    branch19.add_child(branch20);
    branches_vec.push_back(&branch20);

    // branch21
    ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.05, 0., SEG, 0, 1, 8.77e9, 1.f,
                                        1.f, 2.f);
    std::vector<Point> points_ch21(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch branch21(points_ch21, glm::vec3(0), -20, 0, 20, length);
    branch21.update_points();
    branch19.add_child(branch21);
    branches_vec.push_back(&branch21);

    // --------------------

    // branch22
    ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.07, 0.04, SEG, 0, 1, 8.77e9, 1.f,
                                        1.f, 2.f);
    std::vector<Point> points_ch22(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch branch22(points_ch22, glm::vec3(0), -20, 0, 20, length);
//    int a;
    branch22.update_points();
    branch18.add_child(branch22);
    branches_vec.push_back(&branch22);

    // branch23
    ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.04, 0., SEG, 0, 1, 8.77e9, 1.f,
                                        1.f, 2.f);
    std::vector<Point> points_ch23(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch branch23(points_ch23, glm::vec3(0), 20, 0, 20, length);
//    std::cin >> a;
    branch23.update_points();
    branch22.add_child(branch23);
    branches_vec.push_back(&branch23);

    // --------------------

    // branch24
    ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.09, 0.07, SEG, 0, 1, 8.77e9, 1.f,
                                        1.f, 2.f);
    std::vector<Point> points_ch24(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch branch24(points_ch24, glm::vec3(0), 40, 220, 20, length);
    branch24.update_points();
    branch1.add_child(branch24);
    branches_vec.push_back(&branch24);

    // branch25
    ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.07, 0.05, SEG, 0, 1, 8.77e9, 1.f,
                                        1.f, 2.f);
    std::vector<Point> points_ch25(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch branch25(points_ch25, glm::vec3(0), -20, 0, 20, length);
    branch25.update_points();
    branch24.add_child(branch25);
    branches_vec.push_back(&branch25);

    // branch26
    ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.05, 0., SEG, 0, 1, 8.77e9, 1.f,
                                        1.f, 2.f);
    std::vector<Point> points_ch26(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch branch26(points_ch26, glm::vec3(0), -20, 0, 20, length);
    branch26.update_points();
    branch25.add_child(branch26);
    branches_vec.push_back(&branch26);

    // branch27
    ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.05, 0., SEG, 0, 1, 8.77e9, 1.f,
                                        1.f, 2.f);
    std::vector<Point> points_ch27(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch branch27(points_ch27, glm::vec3(0), 20, 0, 20, length);
    branch27.update_points();
    branch25.add_child(branch27);
    branches_vec.push_back(&branch27);

    std::default_random_engine e(time(nullptr));
    std::uniform_int_distribution<int> rand(0, branches_vec.size() - 1);
    int snows = 5;
//    int index[5];
//    SPlane planes[5];
//    // 20, 22, 1, 17, 9
//    // 21, 19, 7, 2, 11
//    for(int i = 0; i < snows; i++) {
//        bool repeat;
//        do {
//            repeat = false;
//            index[i] = rand(e);
//            for(int j = 0; j < i; j++) {
//                if(index[j] == index[i]) {
//                    repeat = true;
//                    break;
//                }
//            }
//        } while(repeat);
//        std::cout << index[i] << std::endl;
//        new(&planes[i]) SPlane(*branches_vec[index[i]], 45.F, 0.1);
//    }

//    int index[] = {22, 1, 17, 9, 2};
//    int index[] = {21, 18, 6, 2, 11};
    int index[] = {21, 18, 6, 2, 11, 22, 1, 17, 9};
    snows = 9;
    SPlane planes[9];
    for(int i = 0; i < snows; ++i) {
        new(&planes[i]) SPlane(*branches_vec[index[i]], 45.F, 0.1 / (i /*% 4*/ + 1));
    }

//    ps_branch = MyTree::generate_branch(length/2, glm::vec3(0, 0, 1), 0, 0.03, 0.02, SEG, 0, 1, 8.77e9, 1.f,
//                                        1.f, 2.f);
//    std::vector<Point> points_ch_test(ps_branch, ps_branch + SEG + 1);
//    delete ps_branch;
//    SimpleTreeBranch branch_test(points_ch_test, glm::vec3(0), 90, 0, 20, length/2);
//    branch_test.update_points();
//
//    ps_branch = MyTree::generate_branch(length/2, glm::vec3(0, 0, 1), 0, 0.02, 0.01, SEG, 0, 1, 8.77e9, .5f,
//                                        .5f, 2.f);
//    std::vector<Point> points_ch_test_ch(ps_branch, ps_branch + SEG + 1);
//    delete ps_branch;
//    SimpleTreeBranch branch_test_ch(points_ch_test_ch, glm::vec3(0), 0, 0, 20, length/2);
//    branch_test_ch.update_points();
//    branch_test.add_child(branch_test_ch);
//
//    ps_branch = MyTree::generate_branch(length/2, glm::vec3(0, 0, 1), 0, 0.03, 0.02, SEG, 0, 1, 8.77e9, .5f,
//                                        .5f, 2.f);
//    std::vector<Point> points_ch_test2(ps_branch, ps_branch + SEG + 1);
//    delete ps_branch;
//    SimpleTreeBranch branch_test2(points_ch_test2, glm::vec3(0), 90, 0, 20, length/2);
//    branch_test2.update_points();
//
//    ps_branch = MyTree::generate_branch(length/2, glm::vec3(0, 0, 1), 0, 0.02, 0.01, SEG, 0, 1, 8.77e9, .5f,
//                                        .5f, 2.f);
//    std::vector<Point> points_ch_test2_ch(ps_branch, ps_branch + SEG + 1);
//    delete ps_branch;
//    SimpleTreeBranch branch_test2_ch(points_ch_test2_ch, glm::vec3(0), 0, 0, 20, length/2);
//    branch_test2_ch.update_points();
//    branch_test2.add_child(branch_test2_ch);

//    branch1.rot_z = 1;
    ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.05, 0.,
                                        SEG + 10, 0, 1, 8.77e9, 1.f,
                                        1.f, 2.f);
    std::vector<Point> points_ch_snow(ps_branch, ps_branch + SEG + 10 + 1);
    delete ps_branch;
    SimpleTreeBranch branch_snow(points_ch_snow, glm::vec3(0), -70, 0, 30, length);
    branch_snow.update_points();
    snow_plane = new SPlane(branch_snow, 45.F);
//    snow_plane = new SPlane();

    resource_manager::load_shader("shaders/particle.vert", "shaders/particle.frag", nullptr, "Particle");
    resource_manager::load_texture("textures/snow_near.png", true, "Particle");
    const auto particle_shader = resource_manager::get_shader("Particle");
    const auto particle_texture = resource_manager::get_texture("Particle");

    pg = new particle_generator(particle_shader, particle_texture, 15000, glm::vec3(0, 0, 0),
                                particle_mode::random_down);

//    auto ps_branch2 = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.07, 0.05, SEG, 0, 1, 8.77e9, 1.f, 1.f,
//                                              2.f);
////    auto branch = MyTree::Create_Cylinders(ps_branch, SEG + 1, 20);
////    branch.setup_mesh();
//    std::vector<Point> points2(ps_branch2, ps_branch2 + SEG + 1);
//    delete ps_branch2;
//    std::vector<SimpleTreeBranch> branches2;
//    branches2.emplace_back(points2, glm::vec3(0), b_theta, 0, 20, length);
////    SimpleTreeBranch branch(points, glm::vec3(0), 90, 0);
//    SimpleTree tree2(branches2);

//    const float E = 8.77e9,
////            I = glm::pi<float>() * d ^ 4 / 64,
//            L = length;
//    const float smin = 0.5f,
//            smax = 1.f,
//            epsilon3 = 2.f;
    float q = 1.f;

//    for(int i = 0; i <= SEG; ++i) {
//        std::cout << tree.branches[0].points[i].E << ' ';
//    }
//    std::cout << std::endl;

//    tree.branches[0].points[10].position -= glm::vec3(0, 1, 0);
//    tree.branches[0].points[9].position -= glm::vec3(0, 0.5, 0);
//    tree.branches[0].points[8].position -= glm::vec3(0, 0.3, 0);
//    tree.branches[0].update_points();

//    glm::vec3 lightPos = glm::vec3(-10, -10, -10);
    glm::vec3 lightPos = glm::vec3(0, 3, -2);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // 变形、旋转
//    for(auto &point:tree.branches[0].points) {
//        auto rot = glm::rotate(glm::mat4(1), point.position.y / 20, glm::vec3(0, 0, -1));
//        auto pos4 = rot * glm::vec4(point.position, 1);
//        point.position = glm::vec3(pos4.x, pos4.y, pos4.z);
//    }
//    tree.branches[0].update_points();
    bool stop = false;
//    stop = true;
//    float threshold = fabsf(L * cosf(glm::radians(b_theta)));
//    if(threshold < 1e-5) {
//        threshold = L;
//    }
//    std::cout << threshold << std::endl;
//    std::cout << tree.branches[0].points[tree.branches[0].points.size() - 1].position.x << std::endl;
//    Snow snow;
    while(!glfwWindowShouldClose(window)) {
        const auto current_frame = GLfloat(glfwGetTime());
        delta_time += current_frame - last_frame;
        frame_timer += current_frame - last_frame;
        last_frame = current_frame;
        frame_render++;
        if(frame_timer >= 1.0f) {
            std::cout << "render: " << frame_render << " update:" << frame_update << std::endl;
            frame_render = 0;
            frame_update = 0;
            frame_timer -= 1.0f;
        }
        //std::cout << 1 / delta_time << std::endl;
        glfwPollEvents();
        glClearColor(0.1F, 0.1F, 0.1F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(fps_limit <= 0 || delta_time * fps_limit >= 1.0) {
//            pg->update(delta_time, 3000, force);
//            if(glfwGetTime() - last_change_time > FORCE_CLEAR_INTERVAL) {
//                force = glm::vec3(0.0f);
//            }
            // 网格更新
//            tree.branches[0].points[0].position -= glm::vec3(0, 0.01, 0);
//            tree.branches[0].update_points();
//            branch_test.reset();
//            branch_test2.reset();
//            q += 600 * delta_time;
//            branch_test_ch.uniform_load_pressure(q);
//            branch_test2.uniform_load_pressure(q);
//            branch_test.complete_calculate();
//            branch_test2.complete_calculate();
            if(not stop) {
                tree.reset();
                q += 1000 * delta_time;

//                branch5.uniform_load_pressure(q);
//                for(int i = 0; i < snows; i++) {
//                    branches_vec[index[i]]->uniform_load_pressure(q / (i + 1));
//                }
                branch2.uniform_load_pressure(q);

                stop = tree.complete_calculate() or stop;

                if(stop) {
                    std::cout << tree.branches[0].points[tree.branches[0].points.size() - 1].position.x
                              << std::endl;
                }
//                stop = stop or tree2.branches[0].uniform_load_pressure(q);
//                for(int i = 0; i <= SEG; ++i) {
//                    const float I = glm::pi<float>() * powf(tree.branches[0].points[i].radius, 4.f) / 4;
//                    const float x = L - i * L / SEG;
//
//                    float q_vert = q * sinf(glm::radians(b_theta));
////                    float q_hori = q * cosf(glm::radians(b_theta));
//                    float omega = cal_omega(tree.branches[0].points[i].E, I, q_vert, L, x);
//                    float theta = cal_theta(tree.branches[0].points[i].E, I, q_vert, L, x);
//
//                    if(fabsf(omega) >= threshold) {
//                        stop = true;
////                        std::cout << "stop" << std::endl;
//                    }
//
////                if(i == SEG)
////                    std::cout << omega << std::endl;
//
//                    tree.branches[0].points[i].position = glm::vec3(-omega, tree.branches[0].points[i].position.y,
//                                                                    tree.branches[0].points[i].position.z);
//                    tree.branches[0].points[i].rotAngle = -theta;
//
////                if(i == 0)
////                    std::cout << omega << ' ' << theta << std::endl;
//                }
//                tree.branches[0].update_points();
            }

            // Update
            delta_time = 0;
            frame_update++;
        }

        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom),
                                                (float) SCREEN_WIDTH / (float) SCREEN_HEIGHT,
                                                0.1f, 100.0f);
        glm::mat4 view = camera.get_view_matrix();

//        glDepthMask(GL_FALSE);
//        glFrontFace(GL_CCW);
//        skyboxShader.use();
//        skyboxShader.set_matrix4("view", glm::mat4(glm::mat3(view)));
//        skyboxShader.set_matrix4("projection", projection);
//        glBindVertexArray(skyboxVAO);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
//        glDrawArrays(GL_TRIANGLES, 0, 36);
//        glFrontFace(GL_CW);
//        glDepthMask(GL_TRUE);

        // Draw
        // be sure to activate shader when setting uniforms/drawing objects
        shader.use();
        shader.set_vector3f("objectColor", 0.59F, 0.29F, 0.F);
//        shader.set_vector3f("objectColor", 1.f, 1.f, 1.f);
        shader.set_vector3f("lightColor", 1.0f, 1.0f, 1.0f);
        shader.set_vector3f("lightPos", lightPos);
        shader.set_vector3f("viewPos", camera.position);

//        snow.draw(shader);

        // view/projection transformations
        shader.set_matrix4("projection", projection);
        shader.set_matrix4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.5f, -1.5f, -0.5f));
//        model = glm::rotate(model, glm::radians(-90.f), glm::vec3(0, 0, 1));
//        model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));
        shader.set_matrix4("model", model);
//        branch2.draw(model, shader);
        glFrontFace(GL_CW);
//        l_tree.draw(model, shader);
        tree.draw(model, shader);
//        shader.set_vector3f("objectColor", 1.0f, 1.0f, 1.0f);
//        snow_plane->draw(model, shader);
//        shader.set_vector3f("objectColor", 1.0f, 0.5f, 0.31f);
//        if(snow_plane->branch != nullptr)
//            snow_plane->branch->draw(model, shader);
//        shader.set_vector3f("objectColor", 1.0f, 1.0f, 1.0f);
//        for(int i = 0; i < snows; ++i) {
//            planes[i].draw(model, shader);
//        }

//        shader.set_vector3f("objectColor", 1.0f, 0.5f, 0.31f);
//        branch_test.draw(model, shader);
//        branch_test2.draw(model, shader);

//        tree2.draw(model, shader);
//        tree.draw(shader);

//        glFrontFace(GL_CCW);
//        pg->draw();

//        for(int i = 0; i < branch_num; i++) {
//            glm::mat4 model_branch = glm::mat4(1.0f);
//            model_branch = glm::translate(model_branch, glm::vec3(.1f, .1f + .2f * i, 0));
//            model_branch = glm::rotate(model_branch, glm::radians(360.f * i / branch_num), glm::vec3(0, 1, 0));
//            model_branch = glm::rotate(model_branch, glm::radians(80.f), glm::vec3(0, 0, -1));
//            shader.use();
//            shader.set_matrix4("model", model_branch);
//            branches[i].draw(shader);
//        }

        // draw skybox as last
        // change depth function so depth test passes when values are equal to depth buffer's content
        glDepthFunc(GL_LEQUAL);
        glFrontFace(GL_CCW);
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.get_view_matrix())); // remove translation from the view matrix
        skyboxShader.set_matrix4("view", view);
        skyboxShader.set_matrix4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

// ReSharper disable once CppParameterMayBeConst
void key_callback(GLFWwindow *window, int key, int scancode, const int action, int mode) {
    // When a user presses the escape key, we set the WindowShouldClose property to true, closing the application
    if(action == GLFW_RELEASE)
        return;
    switch(key) {
        case GLFW_KEY_ESCAPE:
            if(action == GLFW_PRESS)
                glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        case GLFW_KEY_W:
            camera.process_keyboard(camera_movement::FORWARD, delta_time);
            break;
        case GLFW_KEY_A:
            camera.process_keyboard(camera_movement::LEFT, delta_time);
            break;
        case GLFW_KEY_S:
            camera.process_keyboard(camera_movement::BACKWARD, delta_time);
            break;
        case GLFW_KEY_D:
            camera.process_keyboard(camera_movement::RIGHT, delta_time);
            break;
        case GLFW_KEY_1:
            snow_plane->change_mode(1);
            break;
        case GLFW_KEY_2:
            snow_plane->change_mode(2);
            break;
        case GLFW_KEY_3:
            snow_plane->change_mode(3);
            break;
        default:
            break;
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    float xoffset = (float) xpos - last_x;
    float yoffset = last_y - (float) ypos; // 注意这里是相反的，因为y坐标是从底部往顶部依次增大的
    last_x = (float) xpos;
    last_y = (float) ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    camera.process_mouse_movement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.process_mouse_scroll((float) yoffset);
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front)
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(std::vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for(unsigned int i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if(data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB,
                         GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

inline float cal_theta(const float &E, const float &I, const float &q, const float &L, const float &x) {
    return q * (powf(L, 3.f) - powf(x, 3.f)) / (6 * E * I);
}

inline float cal_omega(const float &E, const float &I, const float &q, const float &L, const float &x) {
    return q * (4 * powf(L, 3.f) * x - powf(x, 4.f) - 3 * powf(L, 4.f)) / (24 * E * I);
}
