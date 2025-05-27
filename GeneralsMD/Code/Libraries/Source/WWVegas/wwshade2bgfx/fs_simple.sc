// BGFX fragment shader for simple effect
$input v_normal, v_texcoord0

#include <bgfx_shader.sh>
uniform sampler2D s_texColor;
uniform vec3 u_lightDir;
uniform vec3 u_lightColor;

void main()
{
    vec3 normal = normalize(v_normal);
    vec3 lightDir = normalize(u_lightDir);
    float NdotL = max(dot(normal, lightDir), 0.0);
    vec4 texColor = texture2D(s_texColor, v_texcoord0);
    vec3 color = texColor.rgb * u_lightColor * NdotL;
    gl_FragColor = vec4(color, texColor.a);
}
