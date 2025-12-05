#version 430 core

out vec4 FragColor;

in vec2 fragTexcoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse;
uniform sampler2D gORMI;
uniform sampler2D occlusionTexture;         
uniform sampler2D shadowmapTexture;
uniform samplerCube illuminanceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

uniform vec3 directionalLight;
uniform vec3 cameraPosition;
uniform mat4 mLightMatrix;

//material parameter
uniform int shader = 0;
uniform vec3 ambientColor = vec3(0.0f);
uniform vec3 diffuseColor = vec3(1.0f);
uniform vec3 specularColor = vec3(0.5f);
uniform float specularShininess = 32.0f;
uniform float refractionFactor = 0.0f;
//light parameter
uniform float lightIntensity = 1.0f;
uniform vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
uniform vec3 lightAmbient = vec3(0.0f, 0.0f, 0.0f);

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / max(denom, 0.0000001f);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    // Direct Light의 경우 k 매핑이 다름 (IBL과 다름)
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / max(denom, 0.0000001);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
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
    //shadow = pow(shadow,2.0f);
    if(projCoords.z > 1.0)
        shadow = 0.0;
    return shadow;
}

void main()
{
    vec3 position = texture(gPosition, fragTexcoord).rgb;
    vec3 normal = normalize(texture(gNormal, fragTexcoord).rgb);
    vec3 albedo = texture(gDiffuse, fragTexcoord).rgb;
    float occlusion = texture(occlusionTexture, fragTexcoord).r;
    float roughness = texture(gORMI, fragTexcoord).g;
    float metallic = texture(gORMI, fragTexcoord).b;

    vec3 lightDir = normalize(directionalLight);
    vec3 viewDir = normalize(cameraPosition - position);

    float lightValue = max(dot(normal, lightDir),0.0f);

    vec3 halfwayDir = normalize(viewDir + lightDir);

    float bias = max(0.05f * (1.0 - dot(normal, lightDir)), 0.0005f);
    float shadow = CaculationShadow(position, bias);

    //phong, blinn phong
    float reflectValue = 0.0f;
    if(shader == 0)
        reflectValue = pow(max(dot(normal, halfwayDir),0.0f), specularShininess);
    else if(shader == 1)
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        reflectValue = pow(max(dot(viewDir, reflectDir),0.0f), specularShininess);
    }
    //pbr
    vec3 Lo = vec3(0.0f);
    if(shader ==2)
    {
        float IOR = 1.5f;//Index Of Reflection
        vec3 F0 = vec3(pow((IOR-1)/(IOR+1), 2));//Fresnel equation | 각도에 따른 반사율 
        F0 = mix(F0, albedo, metallic);

        vec3 reflectDir = reflect(-viewDir, normal);
        {
            //point, spot, area light 
            //float distance = length(lightPosition, worldPos);
            //float attenuation = 1.0 / (distance * distance);

            //all luminance
            vec3 radiance = lightIntensity * lightColor;

            //cook-torrance BRDF
            float D = DistributionGGX(normal, halfwayDir, roughness);
            float G   = GeometrySmith(normal, viewDir, lightDir, roughness);
            vec3  F   = FresnelSchlick(max(dot(halfwayDir, viewDir), 0.0), F0);

            vec3 numerator = D * G * F; 
            float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
            vec3 specular = numerator / denominator;
            vec3 kS = F;
            vec3 kD = vec3(1.0) - kS;
            kD *= 1.0 - metallic;
            float NdotL = max(dot(normal, lightDir), 0.0);
            Lo += (kD * albedo / PI + specular) * radiance * NdotL * (1.0f - shadow); 
        }
        // ambient
        vec3 F = fresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), F0, roughness);
        vec3 kS = F;
        vec3 kD = 1.0 - kS;
        kD *= 1.0 - metallic;
        vec3 illuminance = texture(illuminanceMap, normal).rgb;
        vec3 diffuse = illuminance * albedo;
        
        //specular
        const float MAX_REFLECTION_LOD = 4.0;
        vec3 prefilteredColor = textureLod(prefilterMap, reflectDir, roughness * MAX_REFLECTION_LOD).rgb;
        vec2 brdf = texture(brdfLUT, vec2(max(dot(normal, viewDir), 0.0), roughness)).rg;
        vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

        vec3 ambient = occlusion * (kD * diffuse + specular) + lightAmbient;
        vec3 color = ambient + Lo;
        FragColor = vec4(color, 1.0);
    }
    else
    {
        //skybox
        //vec3 I = normalize(position-cameraPosition);
        //float ratio = 1.00 / refractionIndex;
        //vec3 R = refract(I, normal, ratio); //reflect(반사), refract(굴절)
        vec3 R = reflect(-viewDir, normal);
        vec3 skyboxColor = texture(illuminanceMap, R).rgb * refractionFactor;
        //result
        vec3 ambient = occlusion * albedo * lightAmbient;
        vec3 diffuse = albedo * diffuseColor * lightValue * lightIntensity * lightColor;
        vec3 specular = specularColor * lightIntensity * lightColor * reflectValue;
        
        vec3 color = ambient + (1.0 - shadow) * (diffuse + specular) + skyboxColor;

        FragColor = vec4(color, 1.0);
    }
}