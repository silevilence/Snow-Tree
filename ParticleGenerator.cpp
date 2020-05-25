//
// Created by lenovo on 2017/9/13.
//

#include <random>
#include "ParticleGenerator.h"
#include <ctime>
#include <glm/detail/type_mat.hpp>

particle_generator::particle_generator(const Shader shader, const texture2d texture, const GLuint amount,  // NOLINT
                                       const glm::vec3 position, const particle_mode mode)
	: position(position), mode(mode), amount_(amount), shader_(shader), texture_(texture) {
	this->init();
}

void particle_generator::update(const GLfloat dt, const GLuint new_particles_per_second, const glm::vec3 force) {
	// Calculate particles need to be generated at this time slice
	new_particles_ += dt * new_particles_per_second;
	// Add new particles
	//for (GLuint i = 0; i < new_particles_per_second; ++i) {
	while (new_particles_ >= 1.0f) {
		const int unusedparticle = this->first_unused_particle();
		respawn_particle(this->particles_[unusedparticle], this->mode);
		this->particle_count++;
		new_particles_--;
	}
	// Update all particles
	for (GLuint i = 0; i < this->amount_; ++i) {
		auto& p = this->particles_[i];
		if(p.life > 0.0f && p.life <=dt) {
			this->particle_count--;
		}
		p.life -= dt; // reduce life
		if (p.life > 0.0f) {
			// particle is alive, thus update
			p.position += p.velocity * dt;
			p.velocity += force * p.diameter / 8.5f;
			//p.color.a -= dt * 2.5;
			if (p.life < 1.0f) {
				p.color.a = p.life;
			}
			if(p.position.y >= 600) {
				p.life = 0.0f;
				this->particle_count--;
			}
		}
	}
}

// Render all particles
void particle_generator::draw() {
	// Use additive blending to give it a 'glow' effect
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	// ------------------------
	// just 4 debug
	//this->shader_.use();
	//this->shader_.set_vector3f("offset", this->position);
	//this->shader_.set_vector4f("color", glm::vec4(0, 0, 1, 0.5));
	//this->texture_.bind();
	//glBindVertexArray(this->vao_);
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	//glBindVertexArray(0);
	// ------------------------
	//int count = 0;
	for (const auto particle : this->particles_) {
		if (particle.life > 0.0f) {
			this->shader_.use();
			this->shader_.set_vector3f("offset", particle.position);
			this->shader_.set_vector4f("color", particle.color);
			this->shader_.set_float("scale", particle.diameter);
			auto model = glm::mat4(1);
			model = translate(model, particle.position + this->position);
			model = scale(model, glm::vec3(particle.diameter, particle.diameter, 0));
			this->shader_.set_matrix4("model", model);
			this->texture_.bind();
			glBindVertexArray(this->vao_);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
			//count++;
		}
	}
	//std::cout << count << std::endl;

	// Don't forget to reset to default blending mode
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void particle_generator::init() {
	srand(time(nullptr));
	// Set up mesh and attribute properties
	GLuint vbo;
	GLfloat particle_quad[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, &this->vao_);
	glGenBuffers(1, &vbo);
	glBindVertexArray(this->vao_);
	// Fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof particle_quad, particle_quad, GL_STATIC_DRAW);
	// Set mesh attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
	glBindVertexArray(0);
	// shader settings
	const auto projection = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
	//const auto proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, -0.1f, 100.0f);
	const auto view = lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	this->shader_.use().set_matrix4("projection", projection);
	this->shader_.set_matrix4("view", view);
	this->shader_.set_integer("sprite", 0);

	new_particles_ = 0;
	// Create this->amount default particle instances
	for (GLuint i = 0; i < this->amount_; ++i)
		this->particles_.emplace_back(particle());
}

// Stores the index of the last particle used (for quick access to next dead particle)
GLuint last_used_particle = 0;

GLuint particle_generator::first_unused_particle() {
	// First search from last used particle, this will usually return almost instantly
	for (auto i = last_used_particle; i < this->amount_; ++i) {
		if (this->particles_[i].life <= 0.0f) {
			last_used_particle = i;
			return i;
		}
	}
	// Otherwise, do a linear search
	for (GLuint i = 0; i < last_used_particle; ++i) {
		if (this->particles_[i].life <= 0.0f) {
			last_used_particle = i;
			return i;
		}
	}
	// All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
	last_used_particle = 0;
	return 0;
}

void particle_generator::respawn_particle(particle& particle, particle_mode& mode) {
	//const auto r_color = static_cast<GLfloat>(0.5 + rand() % 100 / 100.0f);
	//const auto g_color = static_cast<GLfloat>(0.5 + rand() % 100 / 100.0f);
	//const auto b_color = static_cast<GLfloat>(0.5 + rand() % 100 / 100.0f);
	//particle.position = this->position;
	particle.position = glm::vec3(rand() % 8000 / 10.0f, -13, rand() % 1000 / 10.0f);
	particle.color = glm::vec4(1, 1, 1, 1);
	particle.life = 13.0f;
	particle.velocity = glm::vec3(rand() % 20 - 10.0f, rand() % 100 + 300.0f, 0); // rand() % 10 - 5.0f);
	switch (mode) {
	case particle_mode::straight_down:
		particle.velocity.x = 0;
		break;
	case particle_mode::random_down:
		break;
	default:
		break;
	}
	// diameter of snow particle
	auto scale = rand() % 1000 / 2000.0f;
	const auto r = rand() % 1000;
	if (r > 950)
		scale += 0.6;
	particle.diameter = 8.5 * scale;
	//particle.velocity = glm::vec3(rand() % 500 - 250.0f, 100.0f, 0.0f);
}
