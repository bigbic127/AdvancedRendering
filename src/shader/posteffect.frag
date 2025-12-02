#version 430 core
out vec4 FragColor;

in vec2 fragTexcoord;

uniform sampler2D screenTexture;
//postEffectType
uniform int postEffecttype = 0;
//toneMapping
uniform float exposure = 2.2f;
uniform int toneType = 0;

const float offset = 1.0 / 300.0;
vec3 Kernel()
{
     vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );
    float kernel[9];
    switch (postEffecttype)
    {
        case 3:
        {
            kernel = float[](
                -1, -1, -1,
                -1,  9, -1,
                -1, -1, -1
            );
        }
        break;
        case 4:
        {
            kernel = float[](
                1.0/16, 2.0/16, 1.0/16,
                2.0/16, 4.0/16, 2.0/16,
                1.0/16, 2.0/16, 1.0/16
            );
        }
        break;
        case 5:
        {
            kernel = float[](
                1,  1, 1,
                1, -8, 1,
                1,  1, 1
            );
        }
        break;
        default:break;
    }
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, fragTexcoord.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    return col;
}

vec3 ToneMapping()
{
    vec3 result;
    return result;
}

vec3 ACESFilm(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}

void main()
{
    vec3 color = texture(screenTexture, fragTexcoord).rgb;
    switch (postEffecttype)
    {
        case 1:
            color = 1.0f-color;
            break;
        case 2:
            float average = (color.x + color.y + color.z) / 3.0;
            color = vec3(average, average, average);
            break;
        case 3:
        case 4:
        case 5:
            color = Kernel();
            break;
        default:break;
    }

    //gray
    //float average = (color.x + color.y + color.z) / 3.0;
    //color = vec3(average, average, average);
    //color = color / (color + vec3(1.0));
    //color = ACESFilm(color);
    FragColor = vec4(pow(color,vec3(1.0/exposure)), 1.0);

}