#version 330

out vec4 color;

uniform sampler2D mytexture;
//uniform vec3 lightDir;
//uniform vec3 ambientColor;

void main()
{
    color = vec4(1, 1, 0.0, 1.0);
}