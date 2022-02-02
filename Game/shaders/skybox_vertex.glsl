#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 proj;
uniform mat4 view;

void main()
{
    TexCoords = vec3(-aPos.x, aPos.yz);
    vec4 pos = proj * vec4(mat3(view) * aPos, 1.0);
    gl_Position = pos.xyww;
}  