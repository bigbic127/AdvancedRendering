#version 430 core
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexcoord;
layout(std140) uniform mCamera
{
    mat4 mView;
    mat4 mProjection;
};
uniform mat4 mModel;
//displacement
uniform bool bDisp = false;
uniform sampler2D dispTexture;
uniform float heightScale = 0.1f;


void main()
{
    vec3 position = vPosition;
    if (bDisp)
    {
        float height = texture(dispTexture, vTexcoord).r;
        float displacement = (height - 0.5f) * heightScale;
        position += vNormal * displacement;
    }
    gl_Position = mProjection * mView * mModel * vec4(position, 1.0f);
}
