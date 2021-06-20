#type vertex

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

layout(std140, binding = 0) uniform Camera
{
    mat4 u_ViewProjection;
};

layout(push_constant) uniform Transform
{
    mat4 u_Model;
} u_Transform;

struct VertexOutput
{
    vec3 Normal;
    vec2 TexCoord;
};

layout (location = 0) out VertexOutput Output;

void main()
{
    Output.Normal = a_Normal;
    Output.TexCoord = a_TexCoord;

    gl_Position = /*u_ViewProjection * */ u_Transform.u_Model *  vec4(a_Position, 1.0f);
}

#type pixel

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) out vec4 out_Color;

struct VertexOutput
{
    vec3 Normal;
    vec2 TexCoord;
};

layout(location = 0) in VertexOutput Input;

void main()
{
    out_Color = vec4(1, 1, 1, 1);
}
