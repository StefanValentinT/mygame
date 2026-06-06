#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;

in vec3 vNormal;
in vec3 vTangent;
in vec3 vBitangent;

struct Material {
	sampler2D diffuse1;
	sampler2D specular1;
	sampler2D normal1; 
	float shininess;
};
struct Light {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 color;
};

uniform Light light;
uniform Material material;
uniform vec3 viewPos;

void main()
{
	vec3 N = normalize(vNormal);
	vec3 T = normalize(vTangent - dot(vTangent, N) * N);
	vec3 B = cross(N, T);

	mat3 TBN = mat3(T, B, N);

	vec3 normalMapColor = texture(material.normal1, TexCoord).rgb;
	vec3 localNormal = normalize(normalMapColor * 2.0 - 1.0);

	vec3 norm = normalize(TBN * localNormal);

	vec3 diffuseMapColor = vec3(texture(material.diffuse1, TexCoord));
	vec3 ambient = light.ambient * diffuseMapColor * light.color;
	
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * diffuseMapColor * light.color;

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * light.color;

	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);
}