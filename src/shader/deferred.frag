#version 430 core

out vec4 FragColor;

in vec2 fragTexcoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse;
uniform sampler2D gAmbientOcclusion;

uniform sampler2D shadowmapTexture;

uniform vec3 directionalLight;
uniform vec3 cameraPosition;

uniform mat4 mLightMatrix;

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
    vec3 normal = texture(gNormal, fragTexcoord).rgb;
    vec3 diffuse = texture(gDiffuse, fragTexcoord).rgb;
    float occlusion = texture(gAmbientOcclusion, fragTexcoord).r;

    vec3 lightDir = normalize(directionalLight);
    vec3 viewDir = normalize(cameraPosition - position);

    float lightValue = max(dot(normal, lightDir),0.0f);
    //blinn
    vec3 halfwayDir = normalize(lightDir + viewDir); 
    float reflectValue = pow(max(dot(normal, halfwayDir),0.0f), 32);

    vec3 ambient =  diffuse * 0.2f;
    diffuse = diffuse * lightValue;
    vec3 specular = vec3(1.0f) * reflectValue;

    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = CaculationShadow(position, bias);

    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular) * occlusion);

    vec3 color = result;

    FragColor = vec4(pow(color,vec3(1.0f/2.2f)), 1.0);
}