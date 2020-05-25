//
// Created by lenovo on 2017/9/13.
//

#ifndef LEARNOPENGL_PARTICLEGENERATOR_H
#define LEARNOPENGL_PARTICLEGENERATOR_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>

#include "Shader.h"
#include "Texture2D.h"
#include <glm/detail/type_mat.hpp>

// Represents a single particle and its state
struct particle {
	glm::vec3 position, velocity;
	glm::vec4 color;
	GLfloat life;
	GLfloat diameter;

	particle()
		: position(0.0f), velocity(0.0f), color(1.0f), life(0.0f), diameter(8.5f) {
	}
};


enum class particle_mode {
	straight_down,
	random_down,
};


class particle_generator {
public:
	glm::vec3 position;
	int particle_count;
	particle_mode mode;
	// Constructor
	particle_generator(Shader shader, texture2d texture, GLuint amount, glm::vec3 position = glm::vec3(0.0f), particle_mode mode = particle_mode::random_down);

	// Update all particles
	void update(GLfloat dt, GLuint new_particles_per_second, const glm::vec3 force);

	// Render all particles
	void draw();

private:
	// State
	std::vector<particle> particles_;
	GLuint amount_;
	// Render state
	Shader shader_;
	texture2d texture_;
	GLuint vao_;
	GLfloat new_particles_;

	// Initializes buffer and vertex attributes
	void init();

	// Returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
	GLuint first_unused_particle();

	// Respawns particle
	static void respawn_particle(particle& particle, particle_mode& mode);
};


#endif //LEARNOPENGL_PARTICLEGENERATOR_H
