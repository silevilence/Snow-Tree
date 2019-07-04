#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ResourceManager.h"
#include "MyTree.h"
#include "Camera.h"

#include "LSystem.h"

const GLuint SCREEN_WIDTH = 1600;
const GLuint SCREEN_HEIGHT = 900;
GLuint fps_limit = 0;

auto delta_time = 0.0f;

float last_x, last_y;
const float sensitivity = 0.3f;
// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f)); // NOLINT(cert-err58-cpp)

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

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
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader shader;
    shader.compile_from_file("shaders/object.vert", "shaders/object.frag");


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
//    Mesh &&tree = MyTree::Create_Cylinders(points, seg_num + 1, 50);
//
//    tree.setup_mesh(true);
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
////    Mesh &&branch = MyTree::Create_Cylinders(ps_branch, seg_num + 1, 20);
//
////    branch.setup_mesh();
//
//    delete ps_branch;

//    auto tree_str = LSystem::param_iterator("A(0)", 5);
//    std::cout << tree_str << std::endl;
//    auto tree = LSystem::param_l_interpret(tree_str);

    const int SEG = 25;
    const float length = 2;

    // root branch
    auto ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.1, 0.08, SEG, 0, 1, 8.77e9, 0.5f, 1.f,
                                             2.f);
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

    // child1, up
    ps_branch = MyTree::generate_branch(length, glm::vec3(0, 0, 1), 0, 0.08, 0.05, SEG, 0, 1, 8.77e9, 0.5f, 1.f, 2.f);
    std::vector<Point> points_ch1(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch b_ch1(points_ch1, glm::vec3(0), 0, 0, 20, length);
    b_ch1.update_points();
    tree.branches[0].add_child(b_ch1);

    // child2, right
    ps_branch = MyTree::generate_branch(length / 5, glm::vec3(0, 0, 1), 0, 0.08, 0.07, SEG, 0, 1, 8.77e9, 1.f, 1.f,
                                        2.f);
    std::vector<Point> points_ch2(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch b_ch2(points_ch2, glm::vec3(0), b_theta, 0, 20, length / 5);
    b_ch2.update_points();
    tree.branches[0].add_child(b_ch2);

    // child2-1, up
    ps_branch = MyTree::generate_branch(length / 5, glm::vec3(0, 0, 1), 0, 0.04, 0.02, SEG, 0, 1, 8.77e9, 0.7f, 1.f,
                                        2.f);
    std::vector<Point> points_ch21(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch b_ch21(points_ch21, glm::vec3(0), -30, 0, 20, length / 5);
    b_ch21.update_points();
    b_ch2.add_child(b_ch21);

    // child3, right
    ps_branch = MyTree::generate_branch(length / 5, glm::vec3(0, 0, 1), 0, 0.07, 0.06, SEG, 0, 1, 8.77e9, 1.f, 1.f,
                                        2.f);
    std::vector<Point> points_ch3(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch b_ch3(points_ch3, glm::vec3(0), 0, 0, 20, length / 5);
    b_ch3.update_points();
    b_ch2.add_child(b_ch3);

    // child3-1, down
    ps_branch = MyTree::generate_branch(length / 5, glm::vec3(0, 0, 1), 0, 0.04, 0.02, SEG, 0, 1, 8.77e9, 0.7f, 1.f,
                                        2.f);
    std::vector<Point> points_ch31(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch b_ch31(points_ch31, glm::vec3(0), 30, 0, 20, length / 5);
    b_ch31.update_points();
    b_ch3.add_child(b_ch31);

    // child4, right
    ps_branch = MyTree::generate_branch(length / 5, glm::vec3(0, 0, 1), 0, 0.06, 0.05, SEG, 0, 1, 8.77e9, 1.f, 1.f,
                                        2.f);
    std::vector<Point> points_ch4(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch b_ch4(points_ch4, glm::vec3(0), 0, 0, 20, length / 5);
    b_ch4.update_points();
    b_ch3.add_child(b_ch4);

    // child4-1, up
    ps_branch = MyTree::generate_branch(length / 5, glm::vec3(0, 0, 1), 0, 0.04, 0.02, SEG, 0, 1, 8.77e9, 0.7f, 1.f,
                                        2.f);
    std::vector<Point> points_ch41(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch b_ch41(points_ch41, glm::vec3(0), -30, 0, 20, length / 5);
    b_ch41.update_points();
    b_ch4.add_child(b_ch41);

    // child5, right
    ps_branch = MyTree::generate_branch(length / 5, glm::vec3(0, 0, 1), 0, 0.05, 0.04, SEG, 0, 1, 8.77e9, 1.f, 1.f,
                                        2.f);
    std::vector<Point> points_ch5(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch b_ch5(points_ch5, glm::vec3(0), 0, 0, 20, length / 5);
    b_ch5.update_points();
    b_ch4.add_child(b_ch5);

    // child5-1, down
    ps_branch = MyTree::generate_branch(length / 5, glm::vec3(0, 0, 1), 0, 0.04, 0.02, SEG, 0, 1, 8.77e9, 0.7f, 1.f,
                                        2.f);
    std::vector<Point> points_ch51(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch b_ch51(points_ch51, glm::vec3(0), 30, 0, 20, length / 5);
    b_ch51.update_points();
    b_ch5.add_child(b_ch51);

    // child6, right
    ps_branch = MyTree::generate_branch(length / 5, glm::vec3(0, 0, 1), 0, 0.04, 0.03, SEG, 0, 1, 8.77e9, 1.f, 1.f,
                                        2.f);
    std::vector<Point> points_ch6(ps_branch, ps_branch + SEG + 1);
    delete ps_branch;
    SimpleTreeBranch b_ch6(points_ch6, glm::vec3(0), 0, 0, 20, length / 5);
    b_ch6.update_points();
    b_ch5.add_child(b_ch6);


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
    glm::vec3 lightPos = glm::vec3(10, 10, 10);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(fps_limit <= 0 || delta_time * fps_limit >= 1.0) {
            // 网格更新
//            tree.branches[0].points[0].position -= glm::vec3(0, 0.01, 0);
//            tree.branches[0].update_points();
            if(not stop) {
                tree.reset();
                q += 300 * delta_time;
//                stop = true;
//                stop = stop or tree.branches[0].uniform_load_pressure(q, 90);
//                auto ps = b_ch2.points.size() - 1;
//                auto force = b_ch2.points[ps].position - b_ch2.points[0].position;
//                auto angle = glm::degrees(atanf(force.y / force.x)) + b_theta;
//                stop = stop or tree.branches[0].concentrated_load_pressure(q * b_ch2.length, angle);
//                stop = stop or b_ch2.uniform_load_pressure(q);

                stop = b_ch21.uniform_load_pressure(q, 30) or stop;
                stop = b_ch31.uniform_load_pressure(q) or stop;
                stop = b_ch41.uniform_load_pressure(q, 30) or stop;
                stop = b_ch51.uniform_load_pressure(q) or stop;

                stop = tree.complete_calculate() or stop;

                if(stop) {
                    std::cout << tree.branches[0].points[tree.branches[0].points.size() - 1].position.x << std::endl;
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

        // Draw
        // be sure to activate shader when setting uniforms/drawing objects
        shader.use();
        shader.set_vector3f("objectColor", 1.0f, 0.5f, 0.31f);
        shader.set_vector3f("lightColor", 1.0f, 1.0f, 1.0f);
        shader.set_vector3f("lightPos", lightPos);
        shader.set_vector3f("viewPos", camera.position);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float) SCREEN_WIDTH / (float) SCREEN_HEIGHT,
                                                0.1f, 100.0f);
        glm::mat4 view = camera.get_view_matrix();
        shader.set_matrix4("projection", projection);
        shader.set_matrix4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(0, -0.5f, 0));
//        model = glm::rotate(model, glm::radians(45.f), glm::vec3(1, 0, 0));
//        model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));
//        shader.set_matrix4("model", model);
        tree.draw(model, shader);
//        tree2.draw(model, shader);
//        tree.draw(shader);

//        for(int i = 0; i < branch_num; i++) {
//            glm::mat4 model_branch = glm::mat4(1.0f);
//            model_branch = glm::translate(model_branch, glm::vec3(.1f, .1f + .2f * i, 0));
//            model_branch = glm::rotate(model_branch, glm::radians(360.f * i / branch_num), glm::vec3(0, 1, 0));
//            model_branch = glm::rotate(model_branch, glm::radians(80.f), glm::vec3(0, 0, -1));
//            shader.use();
//            shader.set_matrix4("model", model_branch);
//            branches[i].draw(shader);
//        }

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

inline float cal_theta(const float &E, const float &I, const float &q, const float &L, const float &x) {
    return q * (powf(L, 3.f) - powf(x, 3.f)) / (6 * E * I);
}

inline float cal_omega(const float &E, const float &I, const float &q, const float &L, const float &x) {
    return q * (4 * powf(L, 3.f) * x - powf(x, 4.f) - 3 * powf(L, 4.f)) / (24 * E * I);
}
