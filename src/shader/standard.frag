#version 430 core

out vec4 FragColor;
in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexcoord;
//color
uniform vec3 ambientColor = vec3(0.0f);
uniform vec3 diffuseColor = vec3(1.0f);
uniform vec3 specularColor = vec3(0.5f);
//texture
uniform bool bDiffuse = false;
uniform bool bRoughness = false;
uniform bool bMetallic = false;
uniform bool bNormal = false;
uniform sampler2D diffuseTexture;
uniform sampler2D roughnessTexture;
uniform sampler2D metallicTexture;
uniform sampler2D normalTexture;
//propertices
uniform float metallicFactor = 0.0f;
uniform float roughnessFactor = 0.5f;
uniform float shininess = 0.5f;
uniform float specularShininess = 32.0f;
//light
uniform vec3 lightPosition;
uniform float lightIntensity = 1.0f;
uniform vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
uniform vec3 directionalLight = vec3(0.0f, 1.0f, 0.0f);
//camera
uniform vec3 cameraPosition;

void main()
{
    vec3 normal = normalize(fragNormal);
    //light
    vec3 lightDir = normalize(directionalLight);
    float lightValue = max(dot(normal, lightDir),0.0f);
    //specular
    vec3 specularDir = normalize(cameraPosition - fragPosition);
    vec3 reflectDir = reflect(-lightDir, normal);
    float reflectValue = pow(max(dot(specularDir, reflectDir),0.0f), specularShininess);
    //diffuse
    vec3 diffuseTextureColor = vec3(1.0f, 1.0f, 1.0f);
    if(bDiffuse)
        diffuseTextureColor = vec3(texture(diffuseTexture, fragTexcoord).rgb);
    vec3 ambient =  lightColor * ambientColor * diffuseColor * diffuseTextureColor;
    vec3 diffuse = lightIntensity * lightValue * lightColor * diffuseColor * diffuseTextureColor;
    vec3 specular = reflectValue * lightColor * specularColor;
    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0f);
}