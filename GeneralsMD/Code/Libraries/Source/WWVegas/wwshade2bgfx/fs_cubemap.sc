// BGFX fragment shader for cubemap effect
$input v_normal, v_texcoord0

#include <bgfx_shader.sh>
uniform sampler2D s_texColor;
uniform samplerCube s_cubemap;
uniform vec3 u_viewDir;

void main() {
    vec3 normal = normalize(v_normal);
    vec3 viewDir = normalize(u_viewDir);
    vec3 reflectDir = reflect(-viewDir, normal);
    vec4 envColor = textureCube(s_cubemap, reflectDir);
    vec4 texColor = texture2D(s_texColor, v_texcoord0);
    gl_FragColor = mix(texColor, envColor, 0.5); // Simple blend for demonstration
}
