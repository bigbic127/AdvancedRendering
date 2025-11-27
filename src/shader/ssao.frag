#version 430 core

out vec4 FragColor;

in vec2 fragTexcoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D noise;
uniform mat4 cameraView;
uniform mat4 cameraProjection;
uniform vec3 samples[64];
uniform vec2 screenSize;

uniform int kernelSize = 64;
uniform float radius = 20.0f;
uniform float bias = 0.025f;

void main()
{
    vec3 modelPos = texture(gPosition, fragTexcoord).xyz;
    vec3 viewPos = (cameraView * vec4(modelPos , 1.0f)).xyz;
    vec3 modelNormal  = texture(gNormal, fragTexcoord).xyz;
    vec3 viewNormal = normalize(mat3(cameraView) * modelNormal );

    vec3 random = normalize(texture(noise, fragTexcoord * (screenSize/4.0f)).xyz);
    //tbn
    vec3 tangent = normalize(random - viewNormal  * dot(random, viewNormal));
    vec3 bitangent = cross(viewNormal , tangent);
    mat3 TBN = mat3(tangent, bitangent, viewNormal );
    float occlusion = 1.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        vec3 samplePos = TBN * samples[i];
        samplePos = viewPos + samplePos * radius;
        vec4 offset = cameraProjection * vec4(samplePos, 1.0);
        vec3 offsetNDC = offset.xyz/offset.w;
        vec2 offsetUV = offsetNDC.xy * 0.5 + 0.5;
        vec3 modelPosSurface = texture(gPosition, offsetUV).xyz;
        float viewDepthSurface = (cameraView * vec4(modelPosSurface, 1.0)).z;
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(viewPos.z - viewDepthSurface));
        if (samplePos.z <= viewDepthSurface + bias)
            occlusion += 1.0 * rangeCheck;
    }
    float ssao = 1.0 - (occlusion / kernelSize);
    FragColor = vec4(vec3(ssao), 1.0f);
}