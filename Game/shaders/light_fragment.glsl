#version 330
#define MAX_POINT_LIGHTS 7 
#define MAX_SPOT_LIGHTS 7 

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

struct DirLight {
	vec3 direction;
	vec3 color;
};

struct PointLight {
	vec3 position;
	vec3 color;
	float constant;
	float linear;
	float quadratic;
};

struct SpotLight {
	vec3 position;
    vec3 direction;
	vec3 color;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
};

out vec4 final_color;

in vec2 uv0;
in vec3 worldNormal;
in vec3 fragPosition;

uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights [MAX_POINT_LIGHTS];
uniform int n_points;
uniform SpotLight spotLights [MAX_SPOT_LIGHTS];
uniform int n_spots;
uniform sampler2D mytexture;

uniform vec3 ambientColor;
uniform float ambientStrength;

//uniform vec3 pointColor[MAX_LIGHTS];

uniform vec3 viewPosition;

float specularStrength = 0.5;

vec3 CalcDirLight(DirLight light, vec3 N, vec3 viewDir);  
vec3 CalcPointLight(PointLight light, vec3 N, vec3 fragPos, vec3 viewDir); 
vec3 CalcSpotLight(SpotLight light, vec3 N, vec3 fragPos, vec3 viewDir);

vec3 CalcAmbient();
vec3 CalcDiffuse(vec3 N, vec3 L, vec3 color);
vec3 CalcSpecular(vec3 N, vec3 L, vec3 viewDir, vec3 color);

void main()
{
	vec3 viewDirection = normalize(viewPosition - fragPosition);
	vec3 N = normalize(worldNormal);

	vec3 final_light = CalcDirLight(dirLight, N, viewDirection);

	for (int i = 0; i < n_points; i++)
		final_light += CalcPointLight(pointLights[i], N, fragPosition, viewDirection);

	for (int i = 0; i < n_spots; i++)
		final_light += CalcSpotLight(spotLights[i], N, fragPosition, viewDirection);

	final_light += CalcAmbient();

	final_color = vec4(final_light, 1.0);
	final_color.rgb = pow(final_color.rgb, vec3(1/2.2));  // We apply gamma correctionpow(color.rgb, vec3(1/2.2));
} 

vec3 CalcDirLight(DirLight light, vec3 N, vec3 viewDir)
{
	vec3 L = normalize(-light.direction);
	vec3 specular = CalcSpecular(N, L, viewDir, light.color);
	vec3 diffuse = CalcDiffuse(N, L, light.color);

    return (diffuse + specular);
} 

vec3 CalcPointLight(PointLight light, vec3 N, vec3 fragPos, vec3 viewDir)
{
    vec3 L = normalize(light.position - fragPos);

    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 specular = CalcSpecular(N, L, viewDir, light.color);
	vec3 diffuse = CalcDiffuse(N, L, light.color);

    diffuse  *= attenuation;
    specular *= attenuation;
    return (diffuse + specular);
} 

vec3 CalcSpotLight(SpotLight light, vec3 N, vec3 fragPos, vec3 viewDir)
{
	vec3 L = normalize(light.position - fragPos);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    // spotlight intensity
    float theta = dot(L, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 specular = CalcSpecular(N, L, viewDir, light.color);
	vec3 diffuse = CalcDiffuse(N, L, light.color);

    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (diffuse + specular);
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

vec3 CalcDiffuse(vec3 N, vec3 L, vec3 color)
{
	float NdotL = max(dot(N, L), 0.0);
	vec3 dif = NdotL*color;
	if(material.has_diffuse_map == 1) {
		dif =  dif * material.diffuse * vec3(texture(material.diffuse_map, uv0));
	} else {
		dif = dif * material.diffuse;
	}
	return dif;
}

vec3 CalcSpecular(vec3 N, vec3 L, vec3 viewDir, vec3 color)
{
	vec3 reflectDirection = reflect(-L, N);
	float spec = pow(max(dot(viewDir, reflectDirection), 0.0), material.shininess);
	vec3 specular = specularStrength * spec * color;
	if(material.has_specular_map == 1) {
		specular = specular * material.specular * vec3(texture(material.specular_map, uv0));
	} else {
		specular = specular * material.specular;
	}
	return specular;
}