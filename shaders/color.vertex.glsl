#version 110

uniform mat4 mvpMatrix;

attribute vec3 position;
attribute vec4 color;

varying vec4 colorInterpol;

void main() {
	gl_Position = mvpMatrix * vec4(position, 1.0);

	colorInterpol = color;
}
