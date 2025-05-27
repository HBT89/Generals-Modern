// BGFX fragment shader for bump specular effect
$input v_normal, v_texcoord0, v_tangent, v_bitangent

#include <bgfx_shader.sh>
uniform sampler2D s_texColor;
uniform sampler2D s_normalMap;
uniform vec3 u_lightDir;
uniform vec3 u_lightColor;
uniform vec3 u_viewDir;

void main()
{
    vec3 normal = normalize(v_normal);
    vec3 tangent = normalize(v_tangent);
    vec3 bitangent = normalize(v_bitangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    vec3 lightDir = normalize(u_lightDir);
    vec3 viewDir = normalize(u_viewDir);
    vec3 nmap = texture2D(s_normalMap, v_texcoord0).xyz * 2.0 - 1.0;
    vec3 n = normalize(TBN * nmap);
    float NdotL = max(dot(n, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, n);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0); // Hardcoded shininess
    vec4 texColor = texture2D(s_texColor, v_texcoord0);
    vec3 color = texColor.rgb * u_lightColor * NdotL + vec3(1.0) * spec * 0.5;
    gl_FragColor = vec4(color, texColor.a);
}
