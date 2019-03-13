#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ResourceManager.h"
#include "MyTree.h"
#include "Camera.h"

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
    // todo: 不知道为什么要声明顺时针才能让逆时针的点正常渲染
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
//                      Point(glm::vec3(0, 1, 0), 0.9, glm::vec3(1), 0),
//                      Point(glm::vec3(0, 1, 0), 0, glm::vec3(1), 0)};
//    Mesh tree = MyTree::Create_Cylinders(points, 5, 20);

//    Point points[] = {Point(glm::vec3(0, 0, 0), 1, glm::vec3(1,1,0), 25),
//                      Point(glm::vec3(0, 0, 0), 0, glm::vec3(0), 0)};
//    Mesh tree = MyTree::Create_Cylinders(points, 2, 20);

//    int times = 72;
//    auto points = MyTree::generate_circular_helix(1, 10, 0.5, 0.1, times);
//    Mesh tree = MyTree::Create_Cylinders(points, times, 20);

    int seg_num = 10;
    auto points = MyTree::generate_branch(2, glm::vec3(0, 0, -1), 1, 0.5, seg_num, 1, 2);
    Mesh &&tree = MyTree::Create_Cylinders(points, seg_num + 1, 50);

    tree.setup_mesh();
    delete points;

    const int branch_num = 5;
    Mesh branches[branch_num];
    auto ps_branch = MyTree::generate_branch(1.5f, glm::vec3(0, 0, 1), 10, 0.1, seg_num, 2, 2);
    for(auto &branch : branches) {
        branch = std::move(MyTree::Create_Cylinders(ps_branch, seg_num + 1, 20));

        branch.setup_mesh();
    }
//    Mesh &&branch = MyTree::Create_Cylinders(ps_branch, seg_num + 1, 20);

//    branch.setup_mesh();

    delete ps_branch;

    glm::vec3 lightPos = glm::vec3(3, 3, 3);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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
        model = glm::translate(model, glm::vec3(0, 0, 0));
//        model = glm::rotate(model, glm::radians(45.f), glm::vec3(1, 0, 0));
        shader.set_matrix4("model", model);
        tree.draw(shader);

        for(int i = 0; i < branch_num; i++) {
            glm::mat4 model_branch = glm::mat4(1.0f);
            model_branch = glm::translate(model_branch, glm::vec3(0, .1f + .2f * i, 0));
            model_branch = glm::rotate(model_branch, glm::radians(360.f * i / branch_num), glm::vec3(0, 1, 0));
            model_branch = glm::rotate(model_branch, glm::radians(80.f), glm::vec3(0, 0, -1));
            shader.use();
            shader.set_matrix4("model", model_branch);
            branches[i].draw(shader);
        }

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
