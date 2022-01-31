#version 330
//#define MAX_LIGHTS 128

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	sampler2D diffuse_map;
	sampler2D specular_map;
	float shininess;

	int has_diffuse_map;
	int has_specular_map;
	int shininess_alpha;
};

out vec4 color;

in vec2 uv0;
in vec3 worldNormal;
in vec3 fragPosition;

uniform Material material;
uniform sampler2D mytexture;

uniform vec3 ambientColor;
uniform float ambientStrength;

uniform vec3 directionalColor;
uniform vec3 directionalDir;

//uniform vec3 pointColor[MAX_LIGHTS];

uniform vec3 viewPosition;

float specularStrength = 0.5;

vec3 CalcAmbient();
vec3 CalcDiffuse(vec3 N, vec3 L);
vec3 CalcSpecular(vec3 N, vec3 L);

void main()
{
	vec3 N = normalize(worldNormal);
	// Diffuse light
	vec3 L = normalize(directionalDir);
	// Specular
	// Ambient light
	vec3 specular = CalcSpecular(N, L);
	vec3 diffuse = CalcDiffuse(N, L);
	vec3 ambient = CalcAmbient();

	//Point light
	//L = normalize(directionalDir * fragPosition);
	//NdotL = max(dot(N, L), 0.0);
	//vec3 pnt = NdotL*directionalColor;

	vec4 final_light = vec4(diffuse + ambient + specular, 1.0);

	color = final_light;
} 

vec3 CalcAmbient()
{
	vec3 amb = ambientColor*ambientStrength;
	if(material.has_diffuse_map == 1) {
		amb = amb * material.ambient * vec3(texture(material.diffuse_map, uv0));
	} else {
		amb = amb * material.ambient;
	}
	return amb;
}

vec3 CalcDiffuse(vec3 N, vec3 L)
{
	float NdotL = max(dot(N, L), 0.0);
	vec3 dif = NdotL*directionalColor;
	if(material.has_diffuse_map == 1) {
		dif =  dif * material.diffuse * vec3(texture(material.diffuse_map, uv0));
	} else {
		dif = dif * material.diffuse;
	}
	return dif;
}

vec3 CalcSpecular(vec3 N, vec3 L)
{
	vec3 viewDirection = normalize(viewPosition - fragPosition);
	vec3 reflectDirection = reflect(-L, N);
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
	vec3 specular = specularStrength * spec * directionalColor;
	if(material.has_specular_map == 1) {
		specular = specular * material.specular * vec3(texture(material.specular_map, uv0));
	} else {
		specular = specular * material.specular;
	}
	return specular;
}