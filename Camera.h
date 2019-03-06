#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum camera_movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVTY = 0.1f;
const float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera {
public:
	// Camera Attributes
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 world_up;
	// Eular Angles
	float yaw;
	float pitch;
	// Camera options
	float movement_speed;
	float mouse_sensitivity;
	float zoom;

	// Constructor with vectors
	explicit Camera(const glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
	                const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), const float yaw = YAW, const float pitch = PITCH)
		: front(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed(SPEED), mouse_sensitivity(SENSITIVTY), zoom(ZOOM) {
		this->position = position;
		world_up = up;
		this->yaw = yaw;
		this->pitch = pitch;
		update_camera_vectors();
	}

	// Constructor with scalar values
	Camera(const float pos_x, const float pos_y, const float pos_z, const float up_x, const float up_y, const float up_z,
	       const float yaw, const float pitch)
		: front(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed(SPEED), mouse_sensitivity(SENSITIVTY), zoom(ZOOM) {
		position = glm::vec3(pos_x, pos_y, pos_z);
		world_up = glm::vec3(up_x, up_y, up_z);
		this->yaw = yaw;
		this->pitch = pitch;
		update_camera_vectors();
	}

	// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
	glm::mat4 get_view_matrix() const {
		return lookAt(position, position + front, up);
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void process_keyboard(const camera_movement direction, const float delta_time) {
		const auto velocity = movement_speed * delta_time;
		if (direction == FORWARD)
			position += front * velocity;
		if (direction == BACKWARD)
			position -= front * velocity;
		if (direction == LEFT)
			position -= right * velocity;
		if (direction == RIGHT)
			position += right * velocity;
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void process_mouse_movement(float xoffset, float yoffset, const GLboolean constrain_pitch = true) {
		xoffset *= mouse_sensitivity;
		yoffset *= mouse_sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrain_pitch) {
			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Eular angles
		update_camera_vectors();
	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void process_mouse_scroll(const float yoffset) {
		if (zoom >= 1.0f && zoom <= 45.0f)
			zoom -= yoffset;
		if (zoom <= 1.0f)
			zoom = 1.0f;
		if (zoom >= 45.0f)
			zoom = 45.0f;
	}

private:
	// Calculates the front vector from the Camera's (updated) Eular Angles
	void update_camera_vectors() {
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		this->front = normalize(front);
		// Also re-calculate the Right and Up vector
		right = normalize(cross(this->front, world_up));
		// Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		up = normalize(cross(right, this->front));
	}
};
#endif
