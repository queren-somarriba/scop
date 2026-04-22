#version 330 core

out vec4 FragColor;

struct Material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

struct Light {
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};


in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;
in vec3 FaceColor;
in vec3 FragPosLocal;

uniform sampler2D ourTexture;
uniform Light light;
uniform Material material;
uniform vec4 viewPos;
uniform float transition;

vec3 GetTriPlanarTexture(vec3 currNorm)
{
	vec3 blending = abs(currNorm);

	float sharpness = 8.0;
	blending.x = pow(blending.x, sharpness);
	blending.y = pow(blending.y, sharpness);
	blending.z = pow(blending.z, sharpness);

	blending /= (blending.x + blending.y + blending.z);

	float scale = 1.0;

	vec3 xTex = texture(ourTexture, FragPosLocal.yz * scale).rgb;
	vec3 yTex = texture(ourTexture, FragPosLocal.xz * scale).rgb;
	vec3 zTex = texture(ourTexture, FragPosLocal.xy * scale).rgb;

	return xTex * blending.x + yTex * blending.y + zTex * blending.z;
}

vec3 ComputePhong(vec3 baseColor, vec3 norm, vec3 lightDir, vec3 viewDir)
{
	//ambient
	// vec3 ambient = vec3(light.ambient) * baseColor;
	vec3 ambient = vec3(light.ambient) * vec3(material.ambient) * baseColor;

	//diffuse
	float diff = max(dot(norm, lightDir), 0.0);
	// vec3 diffuse = vec3(light.diffuse) * (diff * baseColor);
	vec3 diffuse = vec3(light.diffuse) * vec3(material.diffuse) * diff * baseColor;

	//specular
	vec3 specular = vec3(0.0);

	if (material.shininess > 0.0)
	{
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		specular = vec3(light.specular) * (spec * vec3(material.specular));
	}
	return (ambient + diffuse + specular);
}

void main()
{
	vec3 fdx = dFdx(FragPosLocal);
	vec3 fdy = dFdy(FragPosLocal);
	vec3 faceNormal = normalize(cross(fdx, fdy));
	vec3 norm = normalize(Normal);

	vec3 triPlanarColor = GetTriPlanarTexture(faceNormal);

	//vec3 normWorld = normalize(Normal);
	vec3 lightDir = normalize(vec3(light.position) - FragPos);
	vec3 viewDir = normalize(vec3(viewPos) - FragPos);

	vec3 colorResult = ComputePhong(vec3(FaceColor), norm, lightDir, viewDir);
	vec3 textureResult = ComputePhong(triPlanarColor, norm, lightDir, viewDir);    

	vec3 final = mix(colorResult, textureResult, transition);
	FragColor = vec4(final, 1.0);
	// FragColor = vec4(normalize(Normal) * 0.5 + 0.5, 1.0);
}