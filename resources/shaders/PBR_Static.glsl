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
} u_Renderer;

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

    gl_Position = u_ViewProjection * u_Renderer.u_Model *  vec4(a_Position, 1.0f);
}

    #type pixel

    #version 450
    #extension GL_ARB_separate_shader_objects : enable

layout (location = 0) out vec4 out_Color;

//layout(set = 0, binding = 5) uniform sampler2D u_AlbedoTexture;

struct DirectionalLight
{
    vec3 Direction;
    vec3 Radiance;
    float Multiplier;
};

struct PointLight
{
    vec3 Position;
    float Multiplier;
    vec3 Radiance;
    float MinRadius;
    float Radius;
    float Falloff;
    float LightSize;
    bool CastsShadows;
};

layout(std140, binding = 2) uniform SceneData
{
    DirectionalLight u_DirectionalLight;
    vec3 u_CameraPosition;
};

layout(std140, binding = 4) uniform PointLightData
{
    uint u_PointLightsCount;
    PointLight u_PointLights[16];
};

layout(push_constant) uniform Material
{
    layout(offset = 64) vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
//
//    float EnvMapRotation;
//
//    bool UseNormalMap;
} u_MaterialUniforms;

struct VertexOutput
{
    vec3 Normal;
    vec2 TexCoord;
};

layout(location = 0) in VertexOutput Input;

void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * vec3(1, 1, 1);

    vec3 norm = normalize(Input.Normal);
    vec3 lightDir = normalize(u_DirectionalLight.Direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1, 1, 1);

    vec3 result = (ambient + diffuse) * u_MaterialUniforms.Diffuse;
    out_Color = vec4(result, 1.0);
    //    out_Color = vec4(1, 1, 1, 1);
}
