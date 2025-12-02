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

vec3 method_reinhard(vec3 x) {
    return x / (x + vec3(1.0));
}

vec3 uncharted2_tonemap_partial(vec3 x) {
    float A = 0.15; // Shoulder Strength
    float B = 0.50; // Linear Strength
    float C = 0.10; // Linear Angle
    float D = 0.20; // Toe Strength
    float E = 0.02; // Toe Numerator
    float F = 0.30; // Toe Denominator
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F)) - E/F;
}

vec3 method_filmic(vec3 x)
{
    float exposure_bias = 2.0;
    vec3 curr = uncharted2_tonemap_partial(x * exposure_bias);
    
    vec3 W = vec3(11.2); // Linear White Point Value
    vec3 white_scale = vec3(1.0) / uncharted2_tonemap_partial(W);
    return curr * white_scale;
}

vec3 method_aces(vec3 x) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

vec3 agxDefaultContrastApprox(vec3 x)
{
    vec3 x2 = x * x;
    vec3 x4 = x2 * x2;
    return + 15.5 * x4 * x2
           - 40.14 * x4 * x
           + 31.96 * x4
           - 6.868 * x2 * x
           + 0.4298 * x2
           + 0.1191 * x
           - 0.00232;
}

vec3 method_agx(vec3 x) {
    // 1. Input Transform (Rec.709 primaries)
    // 색 공간을 회전시켜 Hue Skew를 방지 (Inset)
    const mat3 agx_mat = mat3(
        0.842479062253094, 0.042328242253094, 0.042375654705603,
        0.078433599999999, 0.878468636469672, 0.0784336,
        0.079087337746906, 0.079203121277234, 0.879190745294397
    );
    
    vec3 val = agx_mat * x;
    
    // 2. Log2 Encoding (Approximate range compression)
    // AgX의 inset 영역에 맞게 로그 스케일로 변환
    const float min_ev = -12.47393;
    const float max_ev = 4.026069;
    val = clamp(log2(val), min_ev, max_ev);
    val = (val - min_ev) / (max_ev - min_ev);
    
    // 3. Sigmoid Apply
    val = agxDefaultContrastApprox(val);

    return val;
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
    switch (toneType)
    {
        case 1:
            color = method_reinhard(color);
            break; 
        case 2:
            color = method_filmic(color);
            break;
        case 3:
            color = method_aces(color);
            break;
        case 4:
            color = method_agx(color);
        default:break;
    }

    //gray
    //float average = (color.x + color.y + color.z) / 3.0;
    //color = vec3(average, average, average);
    //color = color / (color + vec3(1.0));
    //color = ACESFilm(color);
    FragColor = vec4(pow(color,vec3(1.0/exposure)), 1.0);

}