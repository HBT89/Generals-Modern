// BGFX fragment shader for cubemap effect
$input v_normal, v_texcoord0, v_viewVec

#include <bgfx_shader.sh>
uniform sampler2D s_texColor;
uniform samplerCube s_cubemap;

void main() {
    vec3 normal = normalize(v_normal);
    vec3 viewVec = normalize(v_viewVec);
    vec3 reflectDir = reflect(-viewVec, normal);
    vec4 envColor = textureCube(s_cubemap, reflectDir);
    vec4 texColor = texture2D(s_texColor, v_texcoord0);
    gl_FragColor = texColor * envColor;
}
