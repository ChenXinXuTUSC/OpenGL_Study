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
};

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    // ambient
    vec4 ambient = vec4(light.ambient, 1.0) * texture(material.texture_diffuse1, TexCoords).rgba;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = vec4(light.diffuse, 1.0) * diff * vec4(texture(material.texture_diffuse1, TexCoords).rgba);

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec4 specular = vec4(light.specular, 1.0) * spec * vec4(texture(material.texture_specular1, TexCoords).rgba); 

    FragColor = ambient + diffuse + specular;
} 