// #version 430

// in vec3 v2fColor; 
// in vec3 v2fPosition; 
// in vec2 v2fTexCoord;
// in vec3 v2fNormal;

// layout( location = 0 ) out vec3 oColor;
// uniform vec3 uShininess;

// layout( location = 2 ) uniform vec3 uLightDir;
// layout( location = 3 ) uniform vec3 uLightPos[3]; 
// layout( location = 6 ) uniform vec3 uLightDiffuse[3];
// layout( location = 9 ) uniform vec3 uLightSpecular[3];
// layout( location = 15 ) uniform vec3 uBaseColor;
// layout( location = 17 ) uniform vec3 uSceneAmbient;
// layout( binding = 0 ) uniform sampler2D uTexture;

// void main()
// {
//     vec3 textureColor = texture(uTexture, v2fTexCoord).rgb;
//     vec3 normal = normalize(v2fNormal) * 0.5 - 1.0;
//     vec3 viewDir = normalize(-v2fPosition);
//     vec3 finalColor = vec3(0.0);

//     for (int i = 0; i < 3; ++i)
//     {
//         vec3 lightDir = normalize(uLightPos[i] - v2fPosition);
//         vec3 halfwayDir = normalize(lightDir + viewDir);
        
//         float distance = length(uLightPos[i] - v2fPosition);
//         float attenuation = 1.0 / (distance * distance);
//         float nDotL = max( 0.0, dot( normal, lightDir ) );
//         float spec = pow(max(dot(normal, halfwayDir), 0.0), 24.0);
        
//         vec3 diffuse = nDotL * uLightDiffuse[i] * attenuation;
//         vec3 specular = spec * uLightSpecular[i] * attenuation;

//         finalColor += diffuse + specular;
//     }
//     finalColor = finalColor + uSceneAmbient + textureColor * uBaseColor;
//     oColor = finalColor * v2fColor;
// }



#version 430

in vec3 v2fColor; 
in vec3 v2fPosition; 
in vec2 v2fTexCoord;
in vec3 v2fNormal;

layout(location = 0) out vec3 oColor;

layout(location = 2) uniform vec3 uLightDir;
layout(location = 3) uniform vec3 uLightPos[3]; 
layout(location = 6) uniform vec3 uLightDiffuse[3];
layout(location = 9) uniform vec3 uLightSpecular[3];

layout(location = 15) uniform vec3 uBaseColor;
layout(location = 17) uniform vec3 uSceneAmbient;
layout(binding = 0) uniform sampler2D uTexture;

void main()
{
    vec3 textureColor = texture(uTexture, v2fTexCoord).rgb;
    vec3 normal = normalize(v2fNormal) * 0.5 - 1.0;
    vec3 viewDir = normalize(-v2fPosition);
    vec3 finalColor = vec3(0.0);

    // Loop through the three point lights
    for (int i = 0; i < 3; ++i)
    {
        vec3 lightDir = normalize(uLightPos[i] - v2fPosition);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        
        float distance = length(uLightPos[i] - v2fPosition);
        float attenuation = 1.0 / (distance * distance);
        float nDotL = max(0.0, dot(normal, lightDir));
        float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
        
        vec3 diffuse = nDotL * uLightDiffuse[i] * attenuation;
        vec3 specular = spec * uLightSpecular[i] * attenuation;
 
        finalColor += diffuse + specular;
    }

    // Combine results and calculate final color
    finalColor = finalColor + uSceneAmbient + textureColor * uBaseColor;
    oColor = finalColor * v2fColor;
}

