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
// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

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

    // OpenGL configuration
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_CULL_FACE);
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

    auto delta_time = 0.0;
    auto last_frame = 0.0;

    Point points[] = {Point(glm::vec3(0, 0, 0), 1.2, glm::vec3(1), 0),
                      Point(glm::vec3(0, 0.1, 0), 1, glm::vec3(0), 0),
                      Point(glm::vec3(0, 0.9, 0), 1, glm::vec3(0), 0),
                      Point(glm::vec3(0, 1, 0), 0.9, glm::vec3(1), 0),
                      Point(glm::vec3(0, 1, 0), 0, glm::vec3(1), 0)};
    Mesh tree = MyTree::Create_Cylinders(points, 5, 20);
    tree.setup_mesh();

    glm::vec3 lightPos = glm::vec3(2, 2, 2);

//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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
        model = glm::translate(model, glm::vec3(0, -0.2f, 0));
        model = glm::rotate(model, glm::radians(45.f), glm::vec3(1, 0, 0));
        shader.set_matrix4("model", model);
        tree.draw(shader);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

// ReSharper disable once CppParameterMayBeConst
void key_callback(GLFWwindow *window, int key, int scancode, const int action, int mode) {
    // When a user presses the escape key, we set the WindowShouldClose property to true, closing the application
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}
