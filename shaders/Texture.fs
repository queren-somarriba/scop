#version 330 core

out vec4 FragColor;

struct Illum
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

struct Material
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

struct Light
{
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
uniform Illum	illum;
uniform vec4 viewPos;
uniform float transition;
uniform float Ztrunc;
uniform bool hasUV;

vec3 GetTriPlanarTexture(vec3 currNorm)
{
	vec3 blending = abs(currNorm);

	float sharpness = 100.0;
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
	vec3 MatAmbient = vec3(light.ambient) * vec3(material.ambient) * baseColor;
	vec3 Ambient = vec3(light.ambient) * vec3(illum.ambient) * baseColor;

	//diffuse
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 MatDiffuse = vec3(light.diffuse) * vec3(material.diffuse) * diff * baseColor;
	vec3 Diffuse = vec3(light.diffuse) * vec3(illum.diffuse) * diff * baseColor;

	//specular
	vec3 MatSpecular = vec3(0.0);
	vec3 reflectDir = reflect(-lightDir, norm);

	if (material.shininess > 0.0)
	{
		float MatSpec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		MatSpecular = vec3(light.specular) * (MatSpec * vec3(material.specular));
	}
	float Spec = pow(max(dot(viewDir, reflectDir), 0.0), illum.shininess);
	vec3 Specular = vec3(light.specular) * (Spec * vec3(illum.specular));

	vec3 MatPhong = MatAmbient + MatDiffuse + MatSpecular;
	vec3 Phong = Ambient + Diffuse + Specular;

	return mix(MatPhong, Phong, transition);
}

void main()
{
	if (FragPos.z > Ztrunc)
		discard;
	vec3 fdx = dFdx(FragPosLocal);
	vec3 fdy = dFdy(FragPosLocal);
	vec3 faceNormal = normalize(cross(fdx, fdy));
	vec3 norm = gl_FrontFacing ? normalize(Normal) : -normalize(Normal);

	vec3 texColor = hasUV ? texture(ourTexture, TexCoord).rgb : GetTriPlanarTexture(faceNormal);
	vec3 mixColor = mix(texColor, FaceColor, transition);

	vec3 lightDir = normalize(vec3(light.position) - FragPos);
	vec3 viewDir = normalize(vec3(viewPos) - FragPos);

	vec3 final = ComputePhong(mixColor, norm, lightDir, viewDir);

	FragColor = vec4(final, 1.0);
}