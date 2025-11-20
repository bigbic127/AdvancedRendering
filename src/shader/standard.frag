#version 430 core

out vec4 FragColor;
in OutFrag
{
    vec3 fragPosition;
    vec3 fragNormal;
    vec2 fragTexcoord;
    vec4 fragLightPosition;
} inFrag;

//color
uniform vec3 ambientColor = vec3(0.0f);
uniform vec4 diffuseColor = vec4(1.0f);
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
uniform float shininessFactor = 0.5f;
uniform float specularShininess = 32.0f;
//light //after.. uniform buffer change.
uniform vec3 lightPosition;
uniform float lightIntensity = 1.0f;
uniform vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
uniform vec3 lightAmbient = vec3(0.0f, 0.0f, 0.0f);
uniform vec3 directionalLight = vec3(0.0f, 1.0f, 0.0f);
uniform bool bSkybox = false;
uniform float refractionFactor = 0.0f;
uniform float refractionIndex = 1.52f;
uniform samplerCube skyboxTexture;
//shadow map
uniform bool bShadowmap = false;
uniform sampler2D shadowmapTexture;
//camera
uniform vec3 cameraPosition;
uniform float near = 0.1; 
uniform float far  = 100.0; 
uniform bool bDepth = false;
//stencil
uniform vec3 stencilColor = vec3(0.1f, 1.0f, 0.05f);
uniform bool bStencil = false;

// 0:blinn 1:phong
uniform int shader = 0;
// 0:Plastic 1:Transparent
uniform int type = 0;

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

float CaculationShadow(vec4 fragLightPosition, float bias=0.005f)
{
    vec3 projCoords = fragLightPosition.xyz / fragLightPosition.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowmapTexture, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    //float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
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
    if(bStencil)
    {
        FragColor = vec4(stencilColor, 1.0f);
        return;
    }
    vec3 normal = normalize(inFrag.fragNormal);
    if(bNormal)
    {
        normal = texture(normalTexture, inFrag.fragTexcoord).rgb;
        normal = normalize(normal * 2.0f - 1.0f);
    }
    //light
    vec3 lightDir = normalize(directionalLight);
    float lightValue = max(dot(normal, lightDir),0.0f);
    //specular
    vec3 specularDir = normalize(cameraPosition - inFrag.fragPosition);
    float reflectValue = 0.0f;
    if(shader == 0)//blinn
    {
        vec3 halfwayDir = normalize(lightDir + specularDir); 
        reflectValue = pow(max(dot(normal, halfwayDir),0.0f), specularShininess);
    }else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        reflectValue = pow(max(dot(specularDir, reflectDir),0.0f), specularShininess);
    }
    //diffuse
    vec3 diffuseTextureColor = vec3(1.0f, 1.0f, 1.0f);
    float alpha = 1.0f;
    if (type == 1)
        alpha = diffuseColor.w;
    if(bDiffuse)
    {
        vec4 texture = texture(diffuseTexture, inFrag.fragTexcoord);
        diffuseTextureColor = pow(texture.rgb, vec3(2.2));
        switch (type)
        {
            case 0:break;
            case 1:
            alpha *= texture.a;
            break;
            default: break;
        }
    }
    //skybox
    vec3 skyboxColor = vec3(1.0f, 1.0f, 1.0f);
    if (bSkybox)
    {
        float ratio = 1.00 / refractionIndex;
        vec3 I = normalize(inFrag.fragPosition-cameraPosition);
        //vec3 R = reflect(I, normal);
        vec3 R = refract(I, normal, ratio);
        skyboxColor = texture(skyboxTexture, R).rgb;
    }
    //result
    vec3 ambient =  lightColor * ambientColor * diffuseColor.xyz * diffuseTextureColor + lightAmbient;
    vec3 diffuse = lightIntensity * lightValue * lightColor * diffuseColor.xyz * diffuseTextureColor;
    vec3 specular = reflectValue * lightIntensity * lightColor * specularColor;
    if(bRoughness)
    {
        float specularvalue = texture(roughnessTexture, inFrag.fragTexcoord).r;
        specular = specular* specularvalue;
    }
    vec3 refraction = skyboxColor * refractionFactor;
    //vec3 result = ambient + diffuse + specular + refraction;

    // calculate shadow
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = CaculationShadow(inFrag.fragLightPosition, bias);
    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular + refraction));

    if (alpha < 0.1f && type == 1)
        discard;
    FragColor = vec4(result, alpha);
    if(bDepth)
    {
        float depth = LinearizeDepth(gl_FragCoord.z) / far;
        FragColor = vec4(vec3(depth), 1.0f);
    }
}