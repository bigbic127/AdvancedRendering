#version 430 core

out vec4 FragColor;
in OutFrag
{
    vec3 fragPosition;
    vec3 fragNormal;
    vec2 fragTexcoord;
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
//light
uniform vec3 lightPosition;
uniform float lightIntensity = 1.0f;
uniform vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
uniform vec3 lightAmbient = vec3(0.0f, 0.0f, 0.0f);

uniform vec3 directionalLight = vec3(0.0f, 1.0f, 0.0f);
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

void main()
{
    vec3 normal = normalize(inFrag.fragNormal);
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
        diffuseTextureColor = vec3(texture.rgb);
        switch (type)
        {
            case 0:break;
            case 1:
            alpha *= texture.a;
            break;
            default: break;
        }
    }
    vec3 ambient =  lightColor * ambientColor * diffuseColor.xyz * diffuseTextureColor;
    vec3 diffuse = lightIntensity * lightValue * lightColor * diffuseColor.xyz * diffuseTextureColor;
    vec3 specular = reflectValue * lightIntensity * lightColor * specularColor;
    vec3 result = ambient + diffuse + specular + lightAmbient;
    if (alpha < 0.1f && type == 1)
        discard;
    FragColor = vec4(result, alpha);
    if(bDepth)
    {
        float depth = LinearizeDepth(gl_FragCoord.z) / far;
        FragColor = vec4(vec3(depth), 1.0f);
    }
    if(bStencil)
         FragColor = vec4(stencilColor, 1.0f);
}