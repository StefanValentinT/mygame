#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in mat4 instanceMatrix;

out vec2 TexCoord;
out vec3 FragPos;

out vec3 vNormal;
out vec3 vTangent;
out vec3 vBitangent;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * instanceMatrix * vec4(aPos, 1.0);
	TexCoord = aTexCoord;

	FragPos = vec3(instanceMatrix * vec4(aPos, 1.0));

	mat3 normalMatrix = mat3(transpose(inverse(instanceMatrix)));
	vNormal    = normalize(normalMatrix * aNormal);
	vTangent   = normalize(normalMatrix * aTangent);
	vBitangent = normalize(normalMatrix * aBitangent);
}