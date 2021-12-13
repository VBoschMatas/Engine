#version 330

out vec4 color;

in vec2 uv0;
in vec3 worldNormal;

uniform sampler2D mytexture;
//uniform vec3 lightDir;
//uniform vec3 ambientColor;

void main()
{
	vec3 lightDir = vec3(0.2, 0.2, 0.7);
	vec3 ambientColor = vec3(1.0, 1.0, 1.0);
	vec3 N = normalize(worldNormal);
	//vec3 N = vec3(0.0, 1.0, 0.0);
	vec3 L = normalize(lightDir);

	float NdotL = -dot(N, L);

	if(NdotL >= 0.0){
		color = texture(mytexture, uv0) * vec4(ambientColor*NdotL, 1.0);
	} else {
		color = texture(mytexture, uv0);
	}
	//color = vec4(1.0, 0.5, 0.25, 1.0);
} 
