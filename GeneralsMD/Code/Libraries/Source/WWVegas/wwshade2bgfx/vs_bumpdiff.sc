$input a_position, a_normal, a_tangent, a_bitangent, a_texcoord0, i_data0, i_data1, i_data2, i_data3
$output v_texcoord0, v_lightDir, v_color

#include "bgfx_shader.sh"
uniform vec4 u_lightDir0;

void main()
{
    // Build model matrix from instance data
    mat4 model = mtxFromCols(i_data0, i_data1, i_data2, i_data3);
    mat3 model3x3 = mtxFromCols(i_data0.xyz, i_data1.xyz, i_data2.xyz);

    // Local to world
    vec4 worldPos = mul(model, vec4(a_position, 1.0));
    gl_Position = mul(u_viewProj, worldPos);

    // Tangent space basis
    vec3 tangent = normalize(mul(model3x3, a_tangent));
    vec3 bitangent = normalize(mul(model3x3, a_bitangent));
    vec3 normal = normalize(mul(model3x3, a_normal));

    mat3 TBN = mtxFromCols(tangent, bitangent, normal);

    // Light dir transform to tangent space
    vec3 lightDirWorld = normalize(u_lightDir0.xyz);
    vec3 lightDirTangent = mul(TBN, lightDirWorld);

    // Output varyings
    v_texcoord0 = a_texcoord0;
    v_lightDir = lightDirTangent * 0.5 + 0.5;
    v_color = vec4_splat(1.0);  // Safer in shaderc context

}
