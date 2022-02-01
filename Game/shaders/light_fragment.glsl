#version 330
#define MAX_POINT_LIGHTS 7 
#define MAX_SPOT_LIGHTS 7 
#define PI 3.14159265359

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	sampler2D diffuse_map;
	sampler2D specular_map;

	float metallic;
	float albedo;
	float smoothness;

	int has_diffuse_map;
	int has_specular_map;
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

uniform vec3 viewPosition;

float specularStrength = 0.5;

// DECLARATIONS ####################################################

vec3 CalcDirLight(DirLight light, vec3 N, vec3 V, vec3 f0, vec3 viewDir, vec3 diffuseColor, vec3 specularColor, float smoothness);  
vec3 CalcPointLight(PointLight light,vec3 N, vec3 V, vec3 f0, vec3 viewDir, vec3 diffuseColor, vec3 specularColor, float smoothness); 
vec3 CalcSpotLight(SpotLight light, vec3 N, vec3 V, vec3 f0, vec3 viewDir, vec3 diffuseColor, vec3 specularColor, float smoothness);

vec3 CalcAmbient();

float GGX(vec3 N, vec3 H, float roughness);
float GeometryGGX(float NdotV, float roughness);
float Smith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 Fresnel(float cosTheta, vec3 f0);

// FUNCTIONS ##########################################################

void GetMaterial(out vec4 diffuse, out vec3 specular, out float smoothness)
{
	diffuse    = vec4(material.diffuse, 1.0);
    specular   = material.specular.rgb;
    smoothness = material.smoothness;

    if(material.has_diffuse_map != 0)
    {
        diffuse *= texture(material.diffuse_map, uv0);
    }

    if(material.has_specular_map != 0)
    {
        vec4 tmp = texture(material.specular_map, uv0);
        specular   *= tmp.rgb;
        smoothness *= tmp.a;
    }
}

void main()
{
	vec4 diffuseColor;
	vec3 specularColor;
	float smoothness;

	GetMaterial(diffuseColor, specularColor, smoothness);

	vec3 viewDirection = normalize(viewPosition - fragPosition);
	vec3 N = normalize(worldNormal);
	vec3 V = normalize(viewPosition - fragPosition);

	vec3 f0 = vec3(0.04);
	f0 = f0 * (1 - material.metallic) + material.albedo * material.metallic;

	vec3 Lo = CalcDirLight(dirLight, N, V, f0, viewDirection, diffuseColor.rgb, specularColor.rgb, smoothness);

	for (int i = 0; i < n_points; i++)
		Lo += CalcPointLight(pointLights[i], N, V, f0, viewDirection, diffuseColor.rgb, specularColor.rgb, smoothness);

	for (int i = 0; i < n_spots; i++)
		Lo += CalcSpotLight(spotLights[i], N, V, f0, viewDirection, diffuseColor.rgb, specularColor.rgb, smoothness);

	Lo += CalcAmbient();

	final_color = vec4(Lo, 1.0);
	final_color.rgb = pow(final_color.rgb, vec3(1/2.2));  // We apply gamma correction
} 

vec3 CalcDirLight(DirLight light, vec3 N, vec3 V, vec3 f0, vec3 viewDir, vec3 diffuseColor, vec3 specularColor, float smoothness)
{
	float roughness = (1.0-smoothness)*(1.0-smoothness);
	vec3 L = normalize(-light.direction);
	vec3 H = normalize(V + L);
	vec3 radiance = light.color;

	float NDF = GGX(N, H, roughness);
	float G = Smith(N, V, L, roughness);
	vec3 F = Fresnel(clamp(dot(H, V), 0.0, 1.0), f0);

	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
	vec3 specular = numerator / denominator;
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - material.metallic;

	float NdotL = max(dot(N, L), 0.0);
	//vec3 specular = CalcSpecular(N, L, viewDir, light.color);
	//vec3 diffuse = diffuseColor * NdotL * light.color;

    return (kD * material.albedo / PI + specular * specularColor) * radiance * NdotL * diffuseColor;
} 

vec3 CalcPointLight(PointLight light, vec3 N, vec3 V, vec3 f0, vec3 viewDir, vec3 diffuseColor, vec3 specularColor, float smoothness)
{
    // attenuation
    float distance    = length(light.position - fragPosition);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    float roughness = (1.0-smoothness)*(1.0-smoothness);
	vec3 L = normalize(light.position - fragPosition);
	vec3 H = normalize(V + L);
	vec3 radiance = light.color * attenuation;

	float NDF = GGX(N, H, roughness);
	float G = Smith(N, V, L, roughness);
	vec3 F = Fresnel(clamp(dot(H, V), 0.0, 1.0), f0);

	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
	vec3 specular = numerator / denominator;
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - material.metallic;

	float NdotL = max(dot(N, L), 0.0);

    return (kD * material.albedo / PI + specular * specularColor) * radiance * NdotL * diffuseColor;
} 

vec3 CalcSpotLight(SpotLight light, vec3 N, vec3 V, vec3 f0, vec3 viewDir, vec3 diffuseColor, vec3 specularColor, float smoothness)
{
	vec3 L = normalize(light.position - fragPosition);

    // attenuation
    float distance = length(light.position - fragPosition);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    // spotlight intensity
    float theta = dot(L, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    float roughness = (1.0-smoothness)*(1.0-smoothness);
	vec3 H = normalize(V + L);
	vec3 radiance = light.color * attenuation * intensity;

	float NDF = GGX(N, H, roughness);
	float G = Smith(N, V, L, roughness);
	vec3 F = Fresnel(clamp(dot(H, V), 0.0, 1.0), f0);

	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
	vec3 specular = numerator / denominator;
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - material.metallic;

	float NdotL = max(dot(N, L), 0.0);

    return (kD * material.albedo / PI + specular * specularColor) * radiance * NdotL * diffuseColor;
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

float GGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometryGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float Smith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometryGGX(NdotV, roughness);
    float ggx1 = GeometryGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 Fresnel(float cosTheta, vec3 f0)
{
	return f0 + (1.0 - f0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}