#version 450

layout(location = 0) in vec3 VertexColor;

layout(location = 0) out vec4 OutColor;

void main() {
    OutColor = vec4(VertexColor, 1.0);
}