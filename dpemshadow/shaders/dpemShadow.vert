#version 330 core
layout(location = 0) in vec3 inPos;

uniform float near;
uniform float far;
uniform mat4 model;
uniform vec3 lightPos;
uniform float dir;

out float posZ;
void main(){
    vec3 pos = (model * vec4(inPos, 1.0)).xyz - lightPos;
    float dis = length(pos.xyz);
    pos.z *= dir;
    pos.xyz /= dis; //normalize
    pos.xy = pos.xy / (pos.z + 1.0);
    pos.z = (dis - near) / (far - near) * 2 - 1; // to [-1, 1]
    posZ = pos.z;
    gl_Position = vec4(pos, 1.0);
}