#version 450
#extension GL_EXT_buffer_reference : require

layout (location = 0) out vec4 vertexColor;
layout (location = 1) out vec2 vertexUV;

struct Vertex {
    vec3 position;
    float uv_x;
    vec3 normal;
    float uv_y;
    vec4 color;
};

layout (buffer_reference, std430) readonly buffer VertexBuffer {
    Vertex vertices[];
};

// Push constant block
layout (push_constant) uniform constants {
    mat4 renderMatrix;
    VertexBuffer vertexBuffer;
} PushConstants;

void main() {
    // Load vertex data from device address.
    Vertex v = PushConstants.vertexBuffer.vertices[gl_VertexIndex];
    
    // Output data
    gl_Position = PushConstants.renderMatrix * vec4(v.position, 1.0f);
    vertexColor = v.color;
    vertexUV.x = v.uv_x;
    vertexUV.y = v.uv_y;
}