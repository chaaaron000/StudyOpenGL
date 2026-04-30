#version  330 core

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    vec3 baseColor;
    float shininess;
};

in vec3 normal;
in vec3 position;
in vec2 texCoord;

out vec4 fragColor;

uniform vec3 viewPos;
uniform Light light;
uniform Material material;

void main()
{
    vec3 lightDir = normalize(light.position - position);
    vec3 texColor = texture2D(material.diffuse, texCoord).xyz;
    vec3 color = texColor * material.baseColor;
    
    // 주변광
    vec3 ambient = color * light.ambient;
    
    // 분산광
    vec3 pixelNorm = normalize(normal);
    float diff = max(dot(pixelNorm, lightDir), 0.0);
    vec3 diffuse = diff * color * light.diffuse;
    
    // 반사광
    vec3 specColor = texture2D(material.specular, texCoord).xyz;
    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, pixelNorm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * specColor * light.specular;
    
    vec3 result = ambient + diffuse + specular;
    fragColor = vec4(result, 1.0);
}