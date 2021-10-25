#version 330 core

out vec4 FragColor;
in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1; 
    float shininess;
}; 

struct Light {
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

uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform bool blinn;

void main()
{
    // ambient
    vec3 ambient = light.ambient * texture(material.texture_diffuse1, TexCoords).rgb;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords).rgb);  
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    float spec = 0.0;
    if (blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, norm);  
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords).rgb); 

    // attenuation(衰减)
    float lgtdistance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * lgtdistance + 
                        light.quadratic * (lgtdistance * lgtdistance));

    FragColor = vec4((ambient + diffuse + specular) * attenuation, 1.0);
} 