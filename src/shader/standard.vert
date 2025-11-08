#version 430 core
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexcoord;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 biTangent;
uniform mat4 mModel, mView, mProjection;
out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTexcoord;

void main()
{
    fragPosition = vec3(mModel * vec4(vPosition, 1.0f));
    fragNormal = transpose(inverse(mat3(mModel))) * vNormal;
    fragTexcoord = vTexcoord;

    gl_Position = mProjection * mView * mModel * vec4(vPosition, 1.0f);
}
