#type vertex

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Bitangent;

struct VertexOutput
{
    vec4 Color;
};

layout (location = 0) out VertexOutput Output;

void main()
{
    Output.Color = vec4(a_Normal, 1.0f);

    gl_Position = vec4(a_Position, 1.0f);
}

    #type pixel

    #version 450
    #extension GL_ARB_separate_shader_objects : enable

layout (location = 0) out vec4 color;

struct VertexOutput
{
    vec4 Color;
};

layout(location = 0) in VertexOutput Input;

void main()
{
    color = Input.Color;
}
