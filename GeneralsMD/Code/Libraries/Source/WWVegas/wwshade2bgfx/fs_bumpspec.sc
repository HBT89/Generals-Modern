// BGFX fragment shader for bump specular effect
$input v_texcoord0, v_lightDirTS, v_viewDirTS, v_normal, v_tangent, v_bitangent

#include <bgfx_shader.sh>
uniform sampler2D s_texColor;
uniform sampler2D s_normalMap;
uniform vec4 u_lightColor0;

void main()
{
    // Sample and unpack normal map (tangent space, [0,1] to [-1,1])
    vec3 nmap = texture2D(s_normalMap, v_texcoord0).xyz * 2.0 - 1.0;
    vec3 normalTS = normalize(nmap);

    // Use tangent-space light and view vectors from vertex shader
    vec3 lightDirTS = normalize(v_lightDirTS);
    vec3 viewDirTS = normalize(v_viewDirTS);

    // Diffuse (Lambert)
    float NdotL = max(dot(normalTS, lightDirTS), 0.0);

    // Specular (Blinn-Phong, hardcoded shininess)
    vec3 halfDir = normalize(lightDirTS + viewDirTS);
    float NdotH = max(dot(normalTS, halfDir), 0.0);
    float spec = pow(NdotH, 16.0); // You may want to expose shininess as a uniform

    // Sample color texture
    vec4 texColor = texture2D(s_texColor, v_texcoord0);

    // Combine
    vec3 color = texColor.rgb * u_lightColor0.rgb * NdotL + u_lightColor0.rgb * spec * 0.5;
    gl_FragColor = vec4(color, texColor.a);
}
