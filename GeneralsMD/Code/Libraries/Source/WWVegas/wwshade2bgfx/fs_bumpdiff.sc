$input v_texcoord0, v_lightDir, v_color
#include "bgfx_shader.sh"

uniform sampler2D s_normalMap;
uniform sampler2D s_diffuseMap;

void main()
{
    // Sample and convert normal from normal map (from [0,1] to [-1,1])
    vec3 normal = texture2D(s_normalMap, v_texcoord0).xyz * 2.0 - 1.0;

    // Normalize interpolated light direction
    vec3 light = normalize(v_lightDir);

    // Basic Lambertian diffuse lighting
    float diff = max(dot(normal, light), 0.0);

    // Sample base color texture
    vec4 baseColor = texture2D(s_diffuseMap, v_texcoord0);

    // Output final lit color
    bgfx_VoidFrag = vec4(baseColor.rgb * diff, baseColor.a);
}
