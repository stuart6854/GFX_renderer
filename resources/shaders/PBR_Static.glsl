#type vertex

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Bitangent;

layout(std140, binding = 0) uniform Camera
{
    mat4 u_ViewProjection;
};

layout (std140, binding = 1) uniform ShadowData
{
	mat4 u_LightViewProj;
};

layout(push_constant) uniform Transform
{
    mat4 u_Model;
} u_Renderer;

struct VertexOutput
{
    vec3 WorldPosition;
    vec3 Normal;
    vec2 TexCoord;
    mat3 WorldNormals;    

    vec4 ShadowMapCoords;
};

layout (location = 0) out VertexOutput Output;

void main()
{
    Output.WorldPosition = vec3(u_Renderer.u_Model * vec4(a_Position, 1.0));
    Output.Normal = mat3(u_Renderer.u_Model) * a_Normal;
    Output.TexCoord = a_TexCoord;

    // Use the "Gram-Schmidt" process to re-orthogonalize the WorldNormals.
    // Slightly improves the normal mapping quality without too much extra cost.
    vec3 T = normalize(vec3(u_Renderer.u_Model * vec4(a_Tangent, 0.0)));
    vec3 N = normalize(vec3(u_Renderer.u_Model * vec4(a_Normal, 0.0)));
    //Re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);
    // Then retrieve perpendicular vector B with the cross product of T and N
    vec3 B = cross(N, T);
    Output.WorldNormals = mat3(T, B, N);

    Output.ShadowMapCoords = u_LightViewProj * vec4(Output.WorldPosition, 1.0);

    gl_Position = u_ViewProjection * u_Renderer.u_Model *  vec4(a_Position, 1.0f);
}

#type pixel

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) out vec4 out_Color;

struct DirectionalLight
{
    vec3 Direction;
    vec3 Color;
    float Padding;
};

struct PointLight
{
    vec3 Position;
    float padding;
    vec3 Color;
    float Constant;
    float Linear;
    float Quadratic;
    float Padding;
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

layout(set = 0, binding = 5) uniform sampler2D u_DiffuseTexture;
layout(set = 0, binding = 6) uniform sampler2D u_NormalTexture;

layout(set = 1, binding = 9) uniform sampler2D u_ShadowMapTexture;

layout(push_constant) uniform Material
{
    layout(offset = 64) vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;

//    float EnvMapRotation;

   bool UseNormalMap;
} u_MaterialUniforms;

struct VertexOutput
{
    vec3 WorldPosition;
    vec3 Normal;
    vec2 TexCoord;
    mat3 WorldNormals;    

    vec4 ShadowMapCoords;
};

layout(location = 0) in VertexOutput Input;

vec3 AmbientColor = vec3(0.05, 0.05, 0.05);
vec3 SpecularColor = vec3(0.5, 0.5, 0.5);

vec3 CalcDirLight_Diffuse(DirectionalLight light, vec3 normal, vec3 objDiffuseColor)
{
    vec3 lightDir = normalize(-light.Direction);
    float diff = max(dot(normal, lightDir), 0.0);
    return light.Color * diff * objDiffuseColor;
}

vec3 CalcDirLight_Specular(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 objSpecularColor)
{
    vec3 lightDir = normalize(-light.Direction);
    // Blinn-Phong
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), 32); // TODO: Replace 32 with material shininess
    return SpecularColor * spec * objSpecularColor;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.Position - fragPos);
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    //Specular
    // Blinn-Phong
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), 32); // TODO: Replace 32 with material shininess
    // Attenuation
    float distance = length(light.Position - fragPos);
    float attenuation = 1.0 / (light.Constant + light.Linear * distance * light.Quadratic * (distance * distance));
    // Combine
    vec3 ambient = AmbientColor * (texture(u_DiffuseTexture, Input.TexCoord).xyz * u_MaterialUniforms.Ambient);
    vec3 diffuse = light.Color * diff * (texture(u_DiffuseTexture, Input.TexCoord).xyz * u_MaterialUniforms.Diffuse);
    vec3 specular = SpecularColor * spec * (texture(u_DiffuseTexture, Input.TexCoord).xyz * u_MaterialUniforms.Specular);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
    // return (ambient + (1.0 - shadow) * (diffuse + specular));
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // Perspective divide. -> [-1,1]
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // [-1,1] -> [0,1]
    projCoords = projCoords * 0.5 + 0.5;
    // Get closest depth value from lights perspective
    float closestDepth = texture(u_ShadowMapTexture, projCoords.xy).r;
    // Get depth of current pixel from lights perspective
    float currentDepth = fragPosLightSpace.z;
    // Check whether current pixel is in shadow
    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;
    return shadow;
}

void main()
{
    vec3 norm = normalize(Input.Normal);
    if(u_MaterialUniforms.UseNormalMap)
    {
        norm = normalize(2.0 * texture(u_NormalTexture, Input.TexCoord).rgb - 1.0);
        norm = normalize(Input.WorldNormals * norm);
    }

    vec3 viewDir = normalize(u_CameraPosition - Input.WorldPosition);

    // Calculate shadow
    float shadow = ShadowCalculation(Input.ShadowMapCoords);

    vec3 ambient = AmbientColor * texture(u_DiffuseTexture, Input.TexCoord).xyz * u_MaterialUniforms.Ambient;

    vec3 objDiffuseColor = texture(u_DiffuseTexture, Input.TexCoord).xyz * u_MaterialUniforms.Diffuse;
    vec3 diffuse = CalcDirLight_Diffuse(u_DirectionalLight, norm, objDiffuseColor);

    vec3 objSpecularColor = texture(u_DiffuseTexture, Input.TexCoord).xyz * u_MaterialUniforms.Diffuse;
    vec3 specular = CalcDirLight_Specular(u_DirectionalLight, norm, viewDir, objSpecularColor);

    vec3 result = (1 - shadow) * texture(u_DiffuseTexture, Input.TexCoord).xyz;

    // Calculate point lights
    for(int i = 0; i < u_PointLightsCount; i++)
        result += CalcPointLight(u_PointLights[i], norm, Input.WorldPosition, viewDir);

    out_Color = vec4(result, 1.0);
    //    out_Color = vec4(1, 1, 1, 1);
}