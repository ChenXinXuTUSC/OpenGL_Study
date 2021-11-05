#version 330 core

out vec4 FragColor;
in INFO_BLOCK
{
    vec3 FragPos;
    vec3 Normal;  
    vec2 TexCoords;
} fs_in;

struct Material 
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Texture
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
};

struct DirLight 
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}; 
struct PointLight 
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // used for decline of light
    // when the distance to light increased.
    float constant;
    float linear;
    float quadratic;
};
#define NR_POINT_LIGHTS 4

uniform bool blinn;
uniform vec3 viewPos;
uniform Material material;
uniform Texture meshTexture;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    // attributes
    vec3 norm = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);

    // phase 1: direction light
    vec3 result = calcDirLight(dirLight, norm, viewDir);
    // phase 2：point lights
    for(int i = 0; i < NR_POINT_LIGHTS; ++i)
        result += calcPointLight(pointLights[i], norm, fs_in.FragPos, viewDir);    
    // phase 3: spot light
    // result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    

    FragColor = vec4(result, 1.0);
} 


vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    float spec = 0.0;
    if (blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);  
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }
    // 合并结果
    vec3 ambient  = material.ambient  * light.ambient *         vec3(texture(meshTexture.texture_diffuse1, fs_in.TexCoords));
    vec3 diffuse  = material.diffuse  * light.diffuse * diff *  vec3(texture(meshTexture.texture_diffuse1, fs_in.TexCoords));
    vec3 specular = material.specular * spec *                  vec3(texture(meshTexture.texture_specular1, fs_in.TexCoords));
    return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    float spec = 0.0;
    if (blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);  
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }

    // 衰减
    float lgtdistance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * lgtdistance + 
                 light.quadratic * (lgtdistance * lgtdistance));   

    // 合并结果
    vec3 ambient  = material.ambient  * light.ambient  *        vec3(texture(meshTexture.texture_diffuse1, fs_in.TexCoords));
    vec3 diffuse  = material.diffuse  * light.diffuse  * diff * vec3(texture(meshTexture.texture_diffuse1, fs_in.TexCoords));
    vec3 specular = material.specular * light.specular * spec * vec3(texture(meshTexture.texture_specular1, fs_in.TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}
