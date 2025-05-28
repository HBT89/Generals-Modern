// BGFX vertex shader for bump specular effect
$input a_position, a_normal, a_tangent, a_bitangent, a_texcoord0
$output v_normal, v_tangent, v_bitangent, v_texcoord0
$output v_lightDirTS, v_viewDirTS

#include <bgfx_shader.sh>

//uniform mat4 u_modelViewProj;
//uniform mat4 u_model;
uniform vec3 u_eyePos;
uniform vec3 u_lightDir0;
uniform vec4 u_lightColor0;

void main()
{
    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
    vec3 worldPos = vec3(mul(u_model, vec4(a_position, 1.0)));
    vec3 N = normalize(vec3(mul((mat3)u_model, a_normal)));
    vec3 T = normalize(vec3(mul((mat3)u_model, a_tangent)));
    vec3 B = normalize(vec3(mul((mat3)u_model, a_bitangent)));
    mat3 TBN = mat3(T, B, N);
    vec3 L = normalize(u_lightDir0);
    vec3 V = normalize(u_eyePos - worldPos);
    v_lightDirTS = TBN * L;
    v_viewDirTS  = TBN * V;
    mat3 model3 = mat3(
        u_model[0].xyz,
        u_model[1].xyz,
        u_model[2].xyz
    );
    v_normal = normalize(model3 * a_normal);
    v_tangent = normalize(model3 * a_tangent);
    v_bitangent = normalize(model3 * a_bitangent);
    v_texcoord0 = a_texcoord0;
}
