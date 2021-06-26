#type vertex

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Bitangent;

layout (std140, binding = 1) uniform ShadowData
{
	mat4 u_LightViewProj;
};

layout(push_constant) uniform Transform
{
    mat4 u_Model;
} u_Renderer;

void main()
{
    gl_Position = u_LightViewProj * u_Renderer.u_Model * vec4(a_Position, 1.0);
}

#type pixel

#version 450
#extension GL_ARB_separate_shader_objects : enable

void main() 
{	
}
