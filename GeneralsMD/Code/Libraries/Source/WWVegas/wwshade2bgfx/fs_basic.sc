// Fragment shader for BGFX pipeline, replacement for legacy WW3D shaders
$input v_normal, v_texcoord0

#include <bgfx_shader.sh>
uniform sampler2D s_texColor;

void main()
{
    vec3 normal = normalize(v_normal);
    vec4 texColor = texture2D(s_texColor, v_texcoord0);
    // Simple diffuse lighting (for demonstration)
    float light = max(dot(normal, vec3(0.0, 0.0, 1.0)), 0.2);
    gl_FragColor = vec4(texColor.rgb * light, texColor.a);
}
