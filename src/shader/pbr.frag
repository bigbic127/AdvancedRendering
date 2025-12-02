#version 430 core

out vec4 FragColor;

in vec2 fragTexcoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse;
uniform sampler2D gRoughness;
uniform sampler2D gAmbientOcclusion;
uniform sampler2D shadowmapTexture;
uniform samplerCube skyboxTexture;

uniform vec3 directionalLight;
uniform vec3 cameraPosition;
uniform mat4 mLightMatrix;

//material parameter
uniform int shader = 0;
uniform vec3 ambientColor = vec3(0.0f);
uniform vec3 diffuseColor = vec3(1.0f);
uniform vec3 specularColor = vec3(0.5f);
//PBR
uniform float roughnessFactor = 0.5f;
uniform float metallicFactor = 0.0f;

//light parameter
uniform float lightIntensity = 1.0f;
uniform vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
uniform vec3 lightAmbient = vec3(0.0f, 0.0f, 0.0f);


float DistributionGGX(vec3 N, vec3 H, float a)
{
    float a2 = a*a;
    float NdotH = max(dot(N,H), 0.0f);
    float NdotH2 = NdotH*NdotH;
    float nom = a2;
    float denom = (NdotH2 * (a2-1.0f)+1.0f);
    denom = PI*denom*denom;
    return nom/denom;
}

float CaculationShadow(vec3 position, float bias=0.005f)
{
    vec4 lightPosition = mLightMatrix * vec4(position, 1.0);
    vec3 projCoords = lightPosition.xyz/lightPosition.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowmapTexture, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    //PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowmapTexture, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowmapTexture, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    if(projCoords.z > 1.0)
        shadow = 0.0;
    return shadow;
}

void main()
{
    vec3 position = texture(gPosition, fragTexcoord).rgb;
    vec3 normal = normalize(texture(gNormal, fragTexcoord).rgb);
    vec3 albedo = texture(gDiffuse, fragTexcoord).rgb;
    float roughness = texture(gRoughness, fragTexcoord).r;
    float occlusion = texture(gAmbientOcclusion, fragTexcoord).r;

    vec3 lightDir = normalize(directionalLight);
    vec3 viewDir = normalize(cameraPosition - position);

    float lightValue = max(dot(normal, lightDir),0.0f);
    //phong, blinn phong
    float reflectValue = 0.0f;
    if(shader == 0)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir); 
        reflectValue = pow(max(dot(normal, halfwayDir),0.0f), specularShininess);
    }else if(shader == 1)
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        reflectValue = pow(max(dot(viewDir, reflectDir),0.0f), specularShininess);
    }
    else
    {
        vec3 halfwayDir = normalize(lightDir + viewDir); 
        reflectValue = DistributionGGX(normal, halfwayDir, roughnessFactor);
    }
    //skybox
    //vec3 I = normalize(position-cameraPosition);
    //float ratio = 1.00 / refractionIndex;
    //vec3 R = refract(I, normal, ratio); //reflect(반사), refract(굴절)
    vec3 R = reflect(-viewDir, normal);
    vec3 skyboxColor = texture(skyboxTexture, R).rgb;
    //result
    vec3 ambient = occlusion * albedo * ambientColor + lightAmbient;
    vec3 diffuse = albedo * diffuseColor * lightValue * lightIntensity * lightColor;
    vec3 specular = specularColor * lightIntensity * lightColor * reflectValue * roughness;
    
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = CaculationShadow(position, bias);
    vec3 result = ambient + (1.0 - shadow) * (diffuse + specular) + (skyboxColor * refractionFactor);

    FragColor = vec4(result, 1.0);
}