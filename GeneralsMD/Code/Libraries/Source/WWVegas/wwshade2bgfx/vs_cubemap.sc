// BGFX vertex shader for cubemap effect
$input a_position, a_normal, a_texcoord0
$output v_normal, v_texcoord0, v_viewVec

#include <bgfx_shader.sh>

//uniform mat4 u_modelViewProj;
//uniform mat4 u_model;
uniform vec3 u_eyePos;

void main() {
    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
    vec3 worldPos = vec3(mul(u_model, vec4(a_position, 1.0)));
    v_normal = normalize(vec3(mul((mat3)u_model, a_normal)));
    v_texcoord0 = a_texcoord0;
    v_viewVec = normalize(u_eyePos - worldPos);
}
