#version 450

layout(set = 0, binding = 0) uniform  SceneData{
    mat4 view;
    mat4 proj;
    mat4 viewproj;
    vec4 ambientColor;
    vec4 sunlightDirection; //w for sun power
    vec4 sunlightColor;
} sceneData;

layout(set = 1, binding = 0) uniform GLTFMaterialData{
    vec4 colorFactors;
    vec4 metalRoughFactors;
} materialData;

layout(set = 1, binding = 1) uniform sampler2D colorTex;
layout(set = 1, binding = 2) uniform sampler2D metalRoughTex;

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inUV;

layout (location = 0) out vec4 outFragColor;

void main()
{
    float lightValue = max(dot(inNormal, sceneData.sunlightDirection.xyz), 0.1f);

    vec3 color = inColor * texture(colorTex,inUV).xyz;
    vec3 ambient = color *  sceneData.ambientColor.xyz;

    outFragColor = vec4(color * lightValue *  sceneData.sunlightColor.w + ambient ,1.0f);
}
