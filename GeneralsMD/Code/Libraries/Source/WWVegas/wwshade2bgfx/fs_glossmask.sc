// BGFX fragment shader for glossmask effect
$input v_normal, v_texcoord0

#include <bgfx_shader.sh>
uniform sampler2D s_texColor;
uniform sampler2D s_glossMask;
uniform vec3 u_lightDir;
uniform vec3 u_lightColor;

void main() {
    vec3 normal = normalize(v_normal);
    vec3 lightDir = normalize(u_lightDir);
    float NdotL = max(dot(normal, lightDir), 0.0);
    vec4 texColor = texture2D(s_texColor, v_texcoord0);
    float gloss = texture2D(s_glossMask, v_texcoord0).r;
    vec3 color = texColor.rgb * u_lightColor * NdotL * gloss;
    gl_FragColor = vec4(color, texColor.a);
}
