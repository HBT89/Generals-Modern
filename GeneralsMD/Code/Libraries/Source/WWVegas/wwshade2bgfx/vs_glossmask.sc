// BGFX vertex shader for glossmask effect
$input a_position, a_normal, a_texcoord0
$output v_normal, v_texcoord0

#include <bgfx_shader.sh>

uniform mat4 u_modelViewProj;
uniform mat4 u_texTransform0;

void main() {
    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
    v_normal = a_normal;
    v_texcoord0 = (u_texTransform0 * vec4(a_texcoord0.xy, 0.0, 1.0)).xy;
}
