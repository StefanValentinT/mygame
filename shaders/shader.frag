#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
struct Light {
	vec3 direction;
  
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Light light;
uniform Material material;
uniform vec3 viewPos;
uniform sampler2D objectTexture;

void main()
{
	vec3 baseColor = vec3(texture(objectTexture, TexCoord));
	vec3 diffuseMapColor = vec3(texture(material.diffuse, TexCoord));
	vec3 specularMapColor = vec3(texture(material.specular, TexCoord));

	vec3 ambient = light.ambient * baseColor * diffuseMapColor;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * baseColor * diffuseMapColor;

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * specularMapColor;

	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);
}