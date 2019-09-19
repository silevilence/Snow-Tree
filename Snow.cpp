//
// Created by lenovo on 2019/9/6.
//

#include "Snow.h"
#include <glm/gtc/matrix_transform.inl>

void Snow::draw(Shader shader) {
    SnowParticle particle;
    particle.radius = 0.05f;
    particle.seg_num = 20;
    particle.generate_mesh();

    for(int i = 0; i < 100; i++) {
        for(int j = 0; j < 100; j++) {
            particle.draw(glm::translate(glm::mat4(1), glm::vec3(i / 100.f, 0, j / 100.f)), shader);
        }
    }
}
