#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 offset;
uniform vec4 color;
uniform float scale;

void main() {
    TexCoords = vertex.zw;
    ParticleColor = color;
    gl_Position = projection * view * model * vec4(vertex.xy, 0, 1.0);
}
