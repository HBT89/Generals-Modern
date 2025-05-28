$input a_position
$output gl_Position

#include <bgfx_shader.sh>

void main()
{
    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
}
