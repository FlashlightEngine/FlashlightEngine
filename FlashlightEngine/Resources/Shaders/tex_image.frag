#version 450

layout (location = 0) in vec4 vertexColor;
layout (location = 1) in vec2 vertexUV;

layout (location = 0) out vec4 FragColor;

layout (set = 0, binding = 0) uniform sampler2D displayTexture;

void main() {
    FragColor = texture(displayTexture, vertexUV);
}