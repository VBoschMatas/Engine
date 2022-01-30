#version 330

out vec4 color;

in vec2 uv0;
in vec3 worldNormal;

uniform sampler2D mytexture;
//uniform vec3 lightDir;
uniform vec3 ambientColor;
uniform float ambientStrength;

void main()
{
	vec3 lightDir = vec3(0.2, 0.2, 0.7);
	vec3 directionalColor = vec3(1.0, 1.0, 1.0);
	vec3 N = normalize(worldNormal);
	vec3 L = normalize(lightDir);

	float NdotL = max(dot(N, L), 0.0);

	vec3 dif = NdotL*directionalColor;
	vec3 amb = ambientColor*ambientStrength;

	vec4 final_light = vec4(amb + dif, 1.0);

	color = texture(mytexture, uv0) * final_light;
} 
