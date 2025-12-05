#version 430 core
out vec4 FragColor;
in vec3 fragPos;

uniform sampler2D hdrTexture;
//const float PI = 3.14159265359;
const vec2 invAtan = vec2(0.1591, 0.3183); // 1/(2*pi), 1/pi

vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
    //float theta = atan(v.z, v.x);
    //float phi   = acos(-v.y);
    //return vec2(theta / (2.0 * PI) + 0.5, phi / PI);
}

void main()
{
    //HDRI Texture
    vec2 uv = SampleSphericalMap(normalize(fragPos));
    vec3 color = texture(hdrTexture, uv).rgb;
    //color = pow(color, vec3(2.2f));
    FragColor = vec4(color, 1.0f);
}