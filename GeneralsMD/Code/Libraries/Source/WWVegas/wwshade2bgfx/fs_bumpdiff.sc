// BGFX fragment shader for bump diffuse effect
$input v_normal, v_texcoord0, v_tangent, v_bitangent

#include <bgfx_shader.sh>
uniform sampler2D s_texColor;
uniform sampler2D s_normalMap;
uniform vec3 u_lightDir;
uniform vec3 u_lightColor;

void main()
{
    vec3 normal = normalize(v_normal);
    vec3 tangent = normalize(v_tangent);
    vec3 bitangent = normalize(v_bitangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    vec3 lightDir = normalize(u_lightDir);
    // Sample and unpack normal map (assume normal map in tangent space, [0,1] to [-1,1])
    vec3 nmap = texture2D(s_normalMap, v_texcoord0).xyz * 2.0 - 1.0;
    vec3 n = normalize(TBN * nmap);
    float NdotL = max(dot(n, lightDir), 0.0);
    vec4 texColor = texture2D(s_texColor, v_texcoord0);
    vec3 color = texColor.rgb * u_lightColor * NdotL;
    gl_FragColor = vec4(color, texColor.a);
}
