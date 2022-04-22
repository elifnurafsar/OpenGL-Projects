#version 150

in vec4 vPosition;
in vec4 vColor;
out vec4 color;

uniform mat4 model_view;
uniform mat4 projection;
uniform mat4 scaling;
uniform mat4 rotateY;
uniform mat4 rotateX;
uniform mat4 translate1;
uniform mat4 translate2;

void main() {
    gl_Position = projection * model_view * rotateY * rotateX * scaling * translate2 * vPosition;
    color = vColor;
}
