#version 430

in vec3 v2fColor;  
in vec2 v2fTexCoord;
in vec3 v2fNormal;

layout( location = 0 ) out vec3 oColor;

layout( location = 2 ) uniform vec3 uLightDir; 
layout( location = 3 ) uniform vec3 uLightDiffuse;
layout( location = 4 ) uniform vec3 uSceneAmbient;

layout( binding = 0 ) uniform sampler2D uTexture;
void main()
{
    vec3 textureColor = texture(uTexture, v2fTexCoord).rgb;
    vec3 normal = normalize(v2fNormal);
    float nDotL = max( 0.0, dot( normal, uLightDir ) );
    oColor = (uSceneAmbient + nDotL * uLightDiffuse) * textureColor;
}