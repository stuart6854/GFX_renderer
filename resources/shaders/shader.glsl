#type vertex

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 a_Position;

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
    vec4 Color;
};

layout (location = 0) out VertexOutput Output;

void main()
{
    Output.Color = vec4(1.0f, 0, 0, 1);

    gl_Position = u_ViewProjection * u_Transform.u_Model * vec4(a_Position, 1.0f);
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
